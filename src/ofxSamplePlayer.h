#ifndef _H_OFXSAMPLEPLAYER
#define _H_OFXSAMPLEPLAYER

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#include "ofMain.h"

typedef struct {
	double minL;
	double maxL;
	double minR;
	double maxR;
} MiniMaxima;

using namespace std;

class ofxSamplePlayer  {

public:

    // constructors/desctructor
    ~ofxSamplePlayer();
    ofxSamplePlayer();
	ofxSamplePlayer(string tmpPath);

	// methods
	string	getPath() { return myPath;}
	bool    getIsLooping();
	bool    getIsLoaded();
	bool    getIsPlaying();
	bool    getIsPaused();
	double	getPosition();
	double  getSpeed();

	void	setPath(string newPath);
	void	setLooping(bool loop);
	void	setPosition(double _position);
	void    setPaused(bool bPaused);
    void    setSpeed(double speed);

	bool	load(string tmpPath);
    void    clear();
	bool	read();
	void	play();
	void    stop();
	double  update();

    bool	save();
    string	getSummary();
    int		getChannels();
    int		getSampleRate();
	long	getLength();

	void	generateWaveForm();
	void	drawWaveForm(int x, int y, int w, int h);
    void	drawWaveForm(int x, int y, int w, int h, vector<MiniMaxima>& WaveForm);
	void	saveWaveForm(string _filename);
	void	loadWaveForm(string _filename);
    
    vector<MiniMaxima>& getWaveForm();
	
    char*   getData();

private:

    vector<MiniMaxima> waveForm;
    
    enum SoundFlags { NONE = 0, LOADED = 1, PLAYING = 2, PAUSED = 4, LOOPING = 8 };

	string  myPath;
	int 	myChunkSize;
	int		mySubChunk1Size;
	short 	myFormat;
	short 	myChannels;
	int   	mySampleRate;
	int   	myByteRate;
	short 	myBlockAlign;
	short 	myBitsPerSample;
	int		myDataSize;
    char* 	myData;
	double	position;
	double	speed;
	double	output;
	bool    isLooping;
	unsigned char soundStatus;

};


#endif // SAMPLER_H
