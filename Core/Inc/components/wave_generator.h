#ifndef WAVE_GENERATOR_H_
#define WAVE_GENERATOR_H_

#define TRIANGLE_POINTS 100
#define SAI_BUFFER_SIZE (TRIANGLE_POINTS * 2)  // *2 for stereo

void Triangle_Wave_Start(void);
void Triangle_Wave_Stop(void);

#endif /* WAVE_GENERATOR_H_ */
