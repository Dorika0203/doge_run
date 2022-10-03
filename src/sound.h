#ifndef __SOUND_H__
#define __SOUND_H__
#include "cgmath.h"		
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <set>
#include "cgut.h"
#include "object.h"

#include "irrKlang\irrKlang.h"

void stop2DEngine();


//준석 추가
irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundEngine* engine3D = nullptr;
irrklang::ISoundSource* sound_src_2D = nullptr;
//irrklang::ISound* sound = nullptr;


//

void play2dsound(const char* path, float volume, bool loop) {
	if (!engine)
		engine = irrklang::createIrrKlangDevice();
	if (!engine) return;
	sound_src_2D = engine->addSoundSourceFromFile(path);
	sound_src_2D->setDefaultVolume(volume);
	engine->play2D(sound_src_2D, loop);

}

void play3dsound(const char* path, object* o, bool play) {

	irrklang::vec3df sound_pos = irrklang::vec3df(o->center.x, o->center.y, o->center.z);
	irrklang::vec3df sound_velocity = irrklang::vec3df(o->velocity.x, o->velocity.y, o->velocity.z);

	if (!o->sound) {
		o->sound = engine->play3D(path, sound_pos, false, true, false);
		//engine->setDopplerEffectParameters(2.0f, 1.0f);
		o->sound->setIsPaused(false);
		o->sound->setMinDistance(1500.0f);
		o->sound->setVolume(100.0f);
	}
	if (o->sound) {
		o->sound->setPosition(sound_pos);
		o->sound->setVelocity(sound_velocity);
	}
}

void drop3d(object* o) {

	if (o->sound) o->sound->drop();
	o->sound = 0;
}
void drop2d() {
	if (sound_src_2D) {
		engine->removeSoundSource(sound_src_2D);
		sound_src_2D = 0;
	}
}
void stop2DEngine() {
	//준석 추가
	engine->drop();
	//요기 까지

}

#endif