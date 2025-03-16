#include <ui.h>

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// UI Stuff
const char* SCREEN_TITLE = "DTMF";
const int SCREEN_WIDTH   = 480;
const int SCREEN_HEIGHT  = 4 * (SCREEN_WIDTH / 3);
UIPixel* screen;

// miniaudio settings
#define FORMAT     ma_format_f32
#define SAMPLERATE 48000
#define CHANNELS   1

ma_waveform lo_wave;
ma_waveform hi_wave;
ma_waveform_config wave_config;

// DTMF
#define ONE       0
#define TWO       1
#define THREE     2
#define FOUR      3
#define FIVE      4
#define SIX       5
#define SEVEN     6
#define EIGHT     7
#define NINE      8
#define ASTERISK  9
#define ZERO     10
#define POUND    11

const int freq_lo[] = { 697,  770,  852,  941};
const int freq_hi[] = {1209, 1336, 1477, 1633};

// miniaudio callback
void play_audio(ma_device* dev, void* obuf, const void* ibuf, ma_uint32 frame_count) {
	float temp[4096];
	ma_waveform_read_pcm_frames(&lo_wave, temp, frame_count, NULL);
	ma_waveform_read_pcm_frames(&hi_wave, obuf, frame_count, NULL);
	for (auto i = 0; i < frame_count; i++)
		((float*)obuf)[i] += temp[i];
}

int UIMain(int argc, char** argv) {
	UIRawInput s;
	UIWidget* root;
	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);

	ma_device_config config;
	ma_device device;

	{ // Setup miniaudio
		config = ma_device_config_init(ma_device_type_playback);
		config.playback.format   = FORMAT;
		config.playback.channels = CHANNELS;
		config.sampleRate        = SAMPLERATE;
		config.dataCallback      = play_audio;
		config.periods           = 1;

		if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
			fprintf(stderr, "ma_device_init failed.\n");
			return -1;
		}

		wave_config = ma_waveform_config_init(
			device.playback.format,
			device.playback.channels,
			device.sampleRate,
			ma_waveform_type_sine,
			0.2,
			0);
		(void) ma_waveform_init(&wave_config, &lo_wave);
		(void) ma_waveform_init(&wave_config, &hi_wave);
	}

	// GUI Tree
	(root = new UIVBox(-1))
		->Children(
			// Row 1
			(new UIHBox(-1))
			->Children(
				new UIButton(ONE),
				new UIButton(TWO),
				new UIButton(THREE)),
			// Row 2
			(new UIHBox(-1))
			->Children(
				new UIButton(FOUR),
				new UIButton(FIVE),
				new UIButton(SIX)),
			// Row 3
			(new UIHBox(-1))
			->Children(
				new UIButton(SEVEN),
				new UIButton(EIGHT),
				new UIButton(NINE)),
			// Row 4
			(new UIHBox(-1))
			->Children(
				new UIButton(ASTERISK),
				new UIButton(ZERO),
				new UIButton(POUND)));
	root->fill_screen = true;

	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);

		// TODO: Handle keyboard input

		if (out.pressed) {
			int digit = out.pressed->id;
			int lo = digit / 3;
			int hi = digit - (lo * 3);

			ma_waveform_set_frequency(&lo_wave, freq_lo[lo]);
			ma_waveform_set_frequency(&hi_wave, freq_hi[hi]);
			ma_device_start(&device);
		} else {
			ma_device_stop(&device);
		}

		UIDraw(scr, root);
		UINativeUpdate();
	}

	// Cleanup miniaudio
	ma_device_uninit(&device);

	return 0;
}
