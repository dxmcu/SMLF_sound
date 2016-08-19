/* Copyright(C) Gaussian Robot. All rights reserved.
 */
/**
 * @file sound_server.cpp
 * @brief sound play server
 * @author lz <lz@gs-robot.com>
 * @version 1.0.0.0
 * @date 2016-02-29
 */
#include <ros/ros.h>
#include <std_msgs/UInt32.h>
#include <diagnostic_msgs/KeyValue.h>
#include <signal.h>
#include <string>
#include <thread>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <SFML/Audio.hpp>

typedef enum {
  STARTPLAY = 0,
  LOOPPLAY,
  STOPPLAY
} PlayCmd;

volatile bool start_play_ = false;
volatile bool insert_play_ = false;
volatile bool stop_play_ = false;
volatile bool loop_play_ = false;
volatile bool under_playing_ = false;
volatile bool sound_stop_flag_ = false;
std::string stop_key = std::string("stop");
std::string start_key = std::string("start");
std::string loop_key = std::string("loop");
std::string insert_key = std::string("insert");
std::string voice_key = std::string("voice");
std::string null_value = std::string("");
std::string sound_file_name_;

ros::Publisher sound_state_publisher;

sf::Sound sound;
pthread_mutex_t lock_;
void lock() { pthread_mutex_lock(&lock_); }
void unlock() { pthread_mutex_unlock(&lock_); }
////////////////////////////////////////////////////////////
/// Play a music
///
////////////////////////////////////////////////////////////
void playMusic(const std::string& filename) {
  // Load an ogg music file
  sf::Music music;
  if (!music.openFromFile(filename))
      return;

  // Display music informations
  std::cout << filename << ":" << std::endl;
  std::cout << " " << music.getDuration().asSeconds() << " seconds"       << std::endl;
  std::cout << " " << music.getSampleRate()           << " samples / sec" << std::endl;
  std::cout << " " << music.getChannelCount()         << " channels"      << std::endl;

  // Play it
  music.play();

  // Loop while the music is playing
  while (music.getStatus() == sf::Music::Playing) {
    // Leave some CPU time for other processes
    sf::sleep(sf::milliseconds(100));

    // Display the playing position
    std::cout << "\rPlaying... " << std::fixed << std::setprecision(2) << music.getPlayingOffset().asSeconds() << " sec   ";
    std::cout << std::flush;
  }
  std::cout << std::endl << std::endl;
}

// 1: playing, 0:stopped
void publish_sound_state(unsigned int state) {
  std_msgs::UInt32 state_msg;
  state_msg.data = state;
  sound_state_publisher.publish(state_msg);
}

////////////////////////////////////////////////////////////
/// Play a sound
///
////////////////////////////////////////////////////////////
void playSound() {
  ROS_WARN("[sound_server] play sound start: file name = %s", sound_file_name_.c_str());
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(sound_file_name_)) {
    ROS_ERROR("[sound_server] loading sound file(%s) failed, please check", sound_file_name_.c_str());
    start_play_ = false;
    return;
  }
  if (!insert_play_) {
    for (int i = 0; i < 35; ++i) {
      if (!start_play_ || insert_play_){
        if (insert_play_) insert_play_ = false;
        break;
      }
      sf::sleep(sf::milliseconds(100));
    }
  } else {
    insert_play_ = false;
  }

  publish_sound_state(1);
  sound.resetBuffer();
  sound.setBuffer(buffer);
  sound.play();

  // Loop while the sound is playing
 while (sound.getStatus() == sf::Sound::Playing && start_play_) {
    // Leave some CPU time for other processes
    sf::sleep(sf::milliseconds(50));
    publish_sound_state(1);
  }
  sound.stop();
  sound.resetBuffer();
  sound.setLoop(false);

  start_play_ = false;
  publish_sound_state(0);
//  ROS_WARN("[sound_server] play sound end!");
}

bool isSame(std::string cmp_str1, std::string cmp_str2) {
   return !std::strcmp(cmp_str1.c_str(), cmp_str2.c_str());
}

void PlaySoundCallBack(const diagnostic_msgs::KeyValue& sound_val) {
  // not null and not under_play_
  if ((isSame(sound_val.key, start_key) || 
       isSame(sound_val.key, loop_key)  ||
       isSame(sound_val.key, voice_key)  ||
       isSame(sound_val.key, insert_key)) && 
       !isSame(sound_val.value ,null_value)) {  //start play
    if ((sound.getStatus() == sf::Sound::Playing || sound.getLoop() || start_play_) && 
        isSame(sound_val.value, sound_file_name_)) { //the same file is playing. just return
//      ROS_INFO("[sound_server] file: %s is playing ,return directly", sound_val.value.c_str());
      return ;
    } 

    //new file, stop the origin one first
    while (sound.getStatus() != sf::Sound::Stopped || start_play_) {
      sound.stop();
      sound.setLoop(false);
      sound.resetBuffer();
//      ROS_INFO("[sound_server] stop origin playing first:\n origin = %s \n current = %s", sound_val.value.c_str(), sound_file_name_.c_str());
      sf::sleep(sf::milliseconds(50));
    }

    if (isSame(sound_val.key, stop_key)) {
      start_play_ = false;
      insert_play_ = false;
//      ROS_INFO("[sound_server] stopped now,return directly");
      return;
    }

    sound_file_name_ = sound_val.value; 
    start_play_ = true;
    if (isSame(sound_val.key, loop_key)) {
      sound.setLoop(true);
      insert_play_ = false;
//      ROS_INFO("[sound_server] loop play called");
    } else if (isSame(sound_val.key, start_key)){
      sound.setLoop(false);
      insert_play_ = false;
//      ROS_INFO("[sound_server] start playing but not loop called");
    } else if (isSame(sound_val.key, insert_key) || isSame(sound_val.key, voice_key)){
      sound.setLoop(false);
      insert_play_ = true;
//      ROS_INFO("[sound_server] start playing but not loop called");
    }
  }
}

void ros_thread() {
  ROS_INFO("[sound_server] ros_thread started");
  ros::NodeHandle n;
  ros::Subscriber play_sound_sub = n.subscribe("/play_sound", 20, PlaySoundCallBack);
  sound_state_publisher = n.advertise<std_msgs::UInt32>("/sound_state", 10);
//  goal_publisher = n.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal", 10);
  ros::Rate loop_rate(20);
  while(1){
      ros::spinOnce();
      loop_rate.sleep();
  }
  ROS_INFO("[sound_server] ros_thread exited");
}

void sigintHandler(int sig)
{
  // TODO(lizhen): Save current pose as default_initial_pose when we're shutting down.
  sound.setLoop(false);
  sound.resetBuffer();
  sound.stop();
//  unlock();
  ros::shutdown();
}

int main(int argc, char** argv) {

  ros::init(argc, argv, "sound_server");
  ROS_INFO("[sound_server] sound_server node started");
  signal(SIGINT, sigintHandler);

//  pthread_mutex_init(&lock_, NULL);
//  ros::NodeHandle nh("~");
//  nh.param("wav_file", wav_path_, wav_file_path);

  std::thread t(ros_thread);
  t.detach();

  ros::NodeHandle n;
  while(n.ok()) {
    if (!start_play_) {
      usleep(10000);
      continue;
    }
    playSound();
    usleep(20000);
  }
  return 0;
}
