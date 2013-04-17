#include "ofxSamplePlayer.h"
#include "ofMain.h"

//--------------------------------------------------------------
ofxSamplePlayer::~ofxSamplePlayer(){
    clear();
}

//--------------------------------------------------------------
ofxSamplePlayer::ofxSamplePlayer(){
    clear();
}

//--------------------------------------------------------------
ofxSamplePlayer::ofxSamplePlayer(string tmpPath){
    load(tmpPath);
}

//--------------------------------------------------------------
void ofxSamplePlayer::setPath(string tmpPath){
    myPath = tmpPath;
}

//--------------------------------------------------------------
void ofxSamplePlayer::setLooping(bool loop){
    if(loop){
        soundStatus |= LOOPING;
    }else{
        soundStatus &= ~LOOPING;
    }
}

//--------------------------------------------------------------
bool ofxSamplePlayer::getIsLooping(){
    if(soundStatus & LOOPING) return true;
    else return false;
}

//--------------------------------------------------------------
bool ofxSamplePlayer::getIsLoaded(){
    if(soundStatus & LOADED) return true;
    else return false;
}

//--------------------------------------------------------------
bool ofxSamplePlayer::getIsPlaying(){
    if(soundStatus & PLAYING) return true;
    else return false;
}

//--------------------------------------------------------------
bool ofxSamplePlayer::getIsPaused(){
    if(soundStatus & PAUSED) return true;
    else return false;
}

//--------------------------------------------------------------
bool ofxSamplePlayer::load(string tmpPath){
    clear();
    myPath = tmpPath;
	bool result = read();
	return result;
}

void ofxSamplePlayer::clear(){
    position = 0;
    speed = 1.0;
    soundStatus = NONE;
    if(myData != NULL){
		delete myData;
		myData = NULL;
	}
    myChunkSize = NULL;
    mySubChunk1Size = NULL;
    myFormat = NULL;
    myChannels = NULL;
    mySampleRate = NULL;
    myByteRate = NULL;
    myBlockAlign = NULL;
    myBitsPerSample = NULL;
    myDataSize = NULL;
}

//--------------------------------------------------------------
void ofxSamplePlayer::generateWaveForm(){

	waveForm.clear();

	bool loopState = getIsLooping();
	setLooping(false);
	bool playState = getIsPlaying();
	double tmpSpeed = getSpeed();
	setSpeed(1.0f);
    play();
	// waveform display method based on this discussion http://answers.google.com/answers/threadview/id/66003.html

    double sampleL, sampleR;
	while ((long)position<getLength()) {

		MiniMaxima mm;
		mm.minL = mm.minR = mm.maxL = mm.maxR = 0;

		for (int i = 0; i < 256; i++){

		    if(myChannels == 1){
                sampleL = update();
                sampleR = sampleL;
		    }else{
                sampleL = update()*0.5;
                sampleR = update()*0.5;
		    }

			mm.minL = MIN(mm.minL, sampleL);
			mm.minR = MIN(mm.minR, sampleR);
			mm.maxL = MAX(mm.maxL, sampleL);
			mm.maxR = MAX(mm.maxR, sampleR);

		}

		waveForm.push_back(mm);

 		//cout << mm.minR << " :: " << mm.maxR << " :: " << mm.minL << " :: " << mm.maxL << endl;
	}

	position = 0;
	setLooping(loopState);
	setSpeed(tmpSpeed);
	if(playState) play();
}

//--------------------------------------------------------------
void ofxSamplePlayer::drawWaveForm(int _x, int _y, int _w, int _h){

	float waveFormZoomX = (float)waveForm.size()/(float)_w;

	glPushMatrix();

	glTranslated(_x, _y, 0);

	for(unsigned int i = 1; i < waveForm.size(); i++){
	    if(myChannels == 1){
            ofSetColor(0, 0, 255);
            ofLine((i-1)/waveFormZoomX, _h + (int)(waveForm.at(i-1).minR*_h), i/waveFormZoomX, _h +  (int)(waveForm.at(i).maxR*_h));
            ofLine(i/waveFormZoomX, _h + (int)(waveForm.at(i).maxR*_h), i/waveFormZoomX, _h + (int) (waveForm.at(i).minR*_h));

            //ofLine((i-1)/waveFormZoomX, (float)_h+waveForm.at(i-1).minR*(float)_h, i/waveFormZoomX, (float)_h+waveForm.at(i).maxR*(float)_h);
           // ofLine((i)/waveFormZoomX, (float)_h+waveForm.at(i).maxR*(float)_h, (i)/waveFormZoomX, (float)_h+waveForm.at(i).minR*(float)_h);
	    } else {
//			if (!singleChannelRepresent) {
				ofSetColor(255, 0, 0);
				//ofLine((i-1)/waveFormZoomX, waveForm.at(i).minR*(float)_h, i/waveFormZoomX, waveForm.at(i).maxR*(float)_h);
				ofLine((i-1)/waveFormZoomX, (int)(waveForm.at(i-1).minL*_h), i/waveFormZoomX, (int)(waveForm.at(i).maxL*_h));
				ofLine(i/waveFormZoomX, (int)(waveForm.at(i).maxL*_h), i/waveFormZoomX, (int) (waveForm.at(i).minL*_h));
//			}
		
            ofSetColor(0, 0, 255);
            ofLine((i-1)/waveFormZoomX, _h + (int)(waveForm.at(i-1).minR*_h), i/waveFormZoomX, _h +  (int)(waveForm.at(i).maxR*_h));
            ofLine(i/waveFormZoomX, _h + (int)(waveForm.at(i).maxR*_h), i/waveFormZoomX, _h + (int) (waveForm.at(i).minR*_h));
	    }
	}

	ofSetColor(0, 255, 0);

    if(myChannels == 1) {
        ofLine(position/(waveForm.size()+1)*256*1, -(float)_h*0.0, position/(waveForm.size()+1)*256*1, (float)_h*2.0);
    }else{
        ofLine(position/(waveForm.size()+1)*256*2, -(float)_h*0.5, position/(waveForm.size()+1)*256*2, (float)_h*1.5);
    }

	glPopMatrix();
}

//--------------------------------------------------------------
void ofxSamplePlayer::stop(){
    position = 0;
    soundStatus &= ~PAUSED;
    soundStatus &= ~PLAYING;
}

//--------------------------------------------------------------
void ofxSamplePlayer::play(){
    if(speed > 0){
        position = 0;
    }else{
        position = getLength();
    }
    soundStatus |= PLAYING;
}

//--------------------------------------------------------------
void ofxSamplePlayer::setPaused(bool bPaused){
    if(bPaused){
        soundStatus |= PAUSED;
    }else{
        soundStatus &= ~PAUSED;
    }
}

//--------------------------------------------------------------
void ofxSamplePlayer::setSpeed(double spd){
    speed = spd;
}

//--------------------------------------------------------------
double ofxSamplePlayer::getSpeed(){
    return speed;
}

//--------------------------------------------------------------
int ofxSamplePlayer::getSampleRate(){
    return mySampleRate;
}

//--------------------------------------------------------------
double ofxSamplePlayer::update(){
    if(!(soundStatus & PLAYING)) return 0;
    if(soundStatus & PAUSED) return 0;
    if(!getIsLoaded()) return 0;

    long length = getLength();
	double remainder;
	short* buffer = (short *)myData;
	position = (position + speed);
	remainder = position - (long) position;

    if((position + 2) * 2 >= myDataSize) return 0;
    
    // check if reached EOF
	if ((long) position > length){
	    if(getIsLooping()){
            position = 0;
	    }else{
            soundStatus &= ~PLAYING;
            return 0;
	    }
	}

    //check if position less than zero (reverse)
	if ((long) position < 0) {
	    if(getIsLooping()) {
            position = length;
	    }else{
            soundStatus &= ~PLAYING;
            return 0;
	    }
	}

	output = (double) ((1.0-remainder) * buffer[1+ (long) position] + remainder * buffer[2+(long) position])/32767.0;//linear interpolation

	return(output);
}

//--------------------------------------------------------------
long ofxSamplePlayer::getLength(){
	long length;
	length=myDataSize*0.5;
	return(length);
}

//--------------------------------------------------------------
double ofxSamplePlayer::getPosition(){
    double pos = position/getLength();
    pos = ofClamp(pos,0.0,1.0);
	return pos;
}

//--------------------------------------------------------------
void ofxSamplePlayer::setPosition(double _position){
	double pct = ofClamp(_position,0.0,1.0);
	position = pct * getLength();
}

//--------------------------------------------------------------
string ofxSamplePlayer::getSummary(){
    ostringstream os;
    os <<   "Format: " << myFormat << 
            " Channels: " << myChannels << 
            " SampleRate: " << mySampleRate << 
            " ByteRate: " << myByteRate << 
            " BlockAlign: " << myBlockAlign << 
            " BitsPerSample: " << myBitsPerSample << 
            " DataSize: " << myDataSize << endl;
    
    return os.str();
}

//--------------------------------------------------------------
int ofxSamplePlayer::getChannels(){
    return myChannels;
}

//--------------------------------------------------------------
vector<MiniMaxima>& ofxSamplePlayer::getWaveForm(){
    return waveForm;
}

//--------------------------------------------------------------
char* ofxSamplePlayer::getData(){
    return myData;
}

//--------------------------------------------------------------
void ofxSamplePlayer::saveWaveForm(string _filename){

	fstream waveFile (_filename.c_str(), ios::out | ios::binary);
	
    waveFile.seekp (0, ios::beg);
	for(int i = 0; i < waveForm.size(); i++) waveFile.write(reinterpret_cast<char *>(&waveForm.at(i)),sizeof(MiniMaxima));
	waveFile.close();

}

//--------------------------------------------------------------
void ofxSamplePlayer::loadWaveForm(string _filename){

	fstream waveFile (_filename.c_str(), ios::in | ios::binary);
	
	// find the end of file and determine vector length
	waveFile.seekg (0, ios::end);
	int waveSize = waveFile.tellg()/sizeof(MiniMaxima);
	
    waveForm.resize(waveSize);
	
    waveFile.seekg (0, ios::beg);
	for(int i = 0; i < waveSize; i++) waveFile.read(reinterpret_cast<char *>(&waveForm.at(i)), sizeof(MiniMaxima));
	waveFile.close();

}

//--------------------------------------------------------------
bool ofxSamplePlayer::save(){
    
    ofToDataPath(myPath);
    fstream myFile (myPath.c_str(), ios::out | ios::binary);

    // write the wav file per the wav file format
    myFile.seekp (0, ios::beg);
    myFile.write ("RIFF", 4);
    myFile.write ((char*) &myChunkSize, 4);
    myFile.write ("WAVE", 4);
    myFile.write ("fmt ", 4);
    myFile.write ((char*) &mySubChunk1Size, 4);
    myFile.write ((char*) &myFormat, 2);
    myFile.write ((char*) &myChannels, 2);
    myFile.write ((char*) &mySampleRate, 4);
    myFile.write ((char*) &myByteRate, 4);
    myFile.write ((char*) &myBlockAlign, 2);
    myFile.write ((char*) &myBitsPerSample, 2);
    myFile.write ("data", 4);
    myFile.write ((char*) &myDataSize, 4);
    myFile.write (myData, myDataSize);

    return true;
    
}

//--------------------------------------------------------------
bool ofxSamplePlayer::read(){
    
    myPath = ofToDataPath(myPath,true).c_str();
    ifstream inFile( myPath.c_str(), ios::in | ios::binary);
	//cout << myPath << endl;
    ofLog(OF_LOG_VERBOSE, "Reading file %s",myPath.c_str());

    if(!inFile.is_open()){
        ofLog(OF_LOG_ERROR,"Error opening file. File not loaded.");
        return false;
    }

    char id[4];
    inFile.read((char*) &id, 4);
    if(strncmp(id,"RIFF",4) != 0){
        ofLog(OF_LOG_ERROR,"Error reading ofxSamplePlayer file. File is not a RIFF (.wav) file");
        return false;
    }

    inFile.seekg(4, ios::beg);
    inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize

    inFile.seekg(16, ios::beg);
    inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size

    //inFile.seekg(20, ios::beg);
    inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM
    if(myFormat != 1){
        ofLog(OF_LOG_ERROR, "File format should be PCM, ofxSamplePlayer file failed to load.");
        return false;
    }

    //inFile.seekg(22, ios::beg);
    inFile.read( (char*) &myChannels, sizeof(short) ); // read the # of channels (1 or 2)

    //inFile.seekg(24, ios::beg);
    inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the Samplerate

    //inFile.seekg(28, ios::beg);
    inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate

    //inFile.seekg(32, ios::beg);
    inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign

    //inFile.seekg(34, ios::beg);
    inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitsperSample

    inFile.seekg(40, ios::beg);
    inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data
    //cout << myDataSize << endl;

    // read the data chunk
    myData = new char[myDataSize];
    inFile.seekg(44, ios::beg);
    inFile.read(myData, myDataSize);

    inFile.close(); // close the input file

    soundStatus |= LOADED;
	
    return true; // this should probably be something more descriptive
    
}
