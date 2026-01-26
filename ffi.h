// C interface definitions for audio effects
// This file is read by CFFI to generate Python bindings

// Discont effect
void discont_init(float pot1, float pot2, float pot3, float pot4);
float discont_step(float in);

// Phaser effect
void phaser_init(float pot1, float pot2, float pot3, float pot4);
float phaser_step(float in);

// Flanger effect
void flanger_init(float pot1, float pot2, float pot3, float pot4);
float flanger_step(float in);

// Echo effect
void echo_init(float pot1, float pot2, float pot3, float pot4);
float echo_step(float in);

// FM effect
void fm_init(float pot1, float pot2, float pot3, float pot4);
float fm_step(float in);

// Magnitude (envelope follower)
void magnitude_init(float pot1, float pot2, float pot3, float pot4);
float magnitude_step(float in);
