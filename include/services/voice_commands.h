#ifndef AEGIS_SERVICES_VOICE_COMMANDS_H
#define AEGIS_SERVICES_VOICE_COMMANDS_H

#include <kernel/types.h>

typedef enum {
    VOICE_CMD_APP_CONTROL = 1,
    VOICE_CMD_SETTINGS = 2,
    VOICE_CMD_SYSTEM = 3,
    VOICE_CMD_WORKSPACE = 4,
    VOICE_CMD_SEARCH = 5,
    VOICE_CMD_HELP = 6,
} voice_cmd_category_t;

typedef struct {
    char phrase[128];
    voice_cmd_category_t category;
    void (*action_fn)(void);
    uint8_t confirmed;
} voice_command_t;

int voice_init(void);
int voice_enable(void);
int voice_disable(void);
int voice_is_enabled(void);

int voice_start_listening(void);
int voice_stop_listening(void);

int voice_process_command(const char *audio_data, uint32_t audio_len);
int voice_recognize_phrase(const char *spoken_phrase, voice_command_t *recognized_cmd);

int voice_execute_command(const char *command_phrase);

int voice_register_custom_command(const voice_command_t *command);
int voice_unregister_custom_command(const char *phrase);

int voice_get_available_commands(voice_command_t *commands, uint32_t *count, uint32_t max_commands);
int voice_set_language(const char *language_code);

int voice_enable_confirmation_prompts(void);
int voice_disable_confirmation_prompts(void);

int voice_train_voice_profile(const char *audio_sample_path);
int voice_enable_speaker_recognition(void);
int voice_disable_speaker_recognition(void);

int voice_set_sensitivity(uint8_t level);
int voice_get_recognition_accuracy(float *accuracy);

#endif
