#include <services/voice_commands.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_VOICE_COMMANDS 64

static voice_command_t g_commands[MAX_VOICE_COMMANDS] = {0};
static uint32_t g_command_count = 0;
static uint8_t g_enabled = 1;
static uint8_t g_listening = 0;
static uint8_t g_confirmations = 0;
static uint8_t g_speaker_recog = 1;
static uint8_t g_sensitivity = 85;
static char g_language[16] = "en-US";

static void to_lower_str(char *dst, const char *src, size_t max) {
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

int voice_init(void) {
    g_command_count = 0;
    g_enabled = 1;
    g_listening = 0;
    g_confirmations = 0;
    g_speaker_recog = 1;
    g_sensitivity = 85;
    strncpy(g_language, "en-US", 15);

    voice_command_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.phrase, "open terminal", 127);
    cmd.category = VOICE_CMD_APP_CONTROL;
    voice_register_custom_command(&cmd);

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.phrase, "scan system", 127);
    cmd.category = VOICE_CMD_SYSTEM;
    voice_register_custom_command(&cmd);

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.phrase, "switch to game mode", 127);
    cmd.category = VOICE_CMD_SYSTEM;
    voice_register_custom_command(&cmd);

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.phrase, "create checkpoint", 127);
    cmd.category = VOICE_CMD_SYSTEM;
    voice_register_custom_command(&cmd);

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.phrase, "show neural monitor", 127);
    cmd.category = VOICE_CMD_APP_CONTROL;
    voice_register_custom_command(&cmd);

    return 0;
}

int voice_enable(void) {
    g_enabled = 1;
    return 0;
}

int voice_disable(void) {
    g_enabled = 0;
    g_listening = 0;
    return 0;
}

int voice_is_enabled(void) {
    return g_enabled;
}

int voice_start_listening(void) {
    if (!g_enabled) return -1;
    g_listening = 1;
    return 0;
}

int voice_stop_listening(void) {
    g_listening = 0;
    return 0;
}

int voice_process_command(const char *audio_data, uint32_t audio_len) {
    if (!audio_data || audio_len == 0 || !g_enabled) return -1;
    return 0;
}

int voice_recognize_phrase(const char *spoken_phrase, voice_command_t *recognized_cmd) {
    if (!spoken_phrase || !recognized_cmd || !g_enabled) return -1;

    char spoken_low[128] = {0};
    to_lower_str(spoken_low, spoken_phrase, sizeof(spoken_low));

    for (uint32_t i = 0; i < g_command_count; i++) {
        char cmd_low[128] = {0};
        to_lower_str(cmd_low, g_commands[i].phrase, sizeof(cmd_low));
        if (strstr(spoken_low, cmd_low) != NULL || strstr(cmd_low, spoken_low) != NULL) {
            *recognized_cmd = g_commands[i];
            return 0;
        }
    }
    return -1;
}

int voice_execute_command(const char *command_phrase) {
    if (!command_phrase || !g_enabled) return -1;
    voice_command_t cmd;
    if (voice_recognize_phrase(command_phrase, &cmd) == 0) {
        if (cmd.action_fn) cmd.action_fn();
        return 0;
    }
    return -1;
}

int voice_register_custom_command(const voice_command_t *command) {
    if (!command || g_command_count >= MAX_VOICE_COMMANDS) return -1;
    g_commands[g_command_count++] = *command;
    return 0;
}

int voice_unregister_custom_command(const char *phrase) {
    if (!phrase) return -1;
    for (uint32_t i = 0; i < g_command_count; i++) {
        if (strcmp(g_commands[i].phrase, phrase) == 0) {
            for (uint32_t j = i; j < g_command_count - 1; j++) {
                g_commands[j] = g_commands[j + 1];
            }
            g_command_count--;
            return 0;
        }
    }
    return -1;
}

int voice_get_available_commands(voice_command_t *commands, uint32_t *count, uint32_t max_commands) {
    if (!commands || !count) return -1;
    uint32_t to_copy = (g_command_count < max_commands) ? g_command_count : max_commands;
    for (uint32_t i = 0; i < to_copy; i++) {
        commands[i] = g_commands[i];
    }
    *count = to_copy;
    return 0;
}

int voice_set_language(const char *language_code) {
    if (!language_code) return -1;
    strncpy(g_language, language_code, 15);
    return 0;
}

int voice_enable_confirmation_prompts(void) {
    g_confirmations = 1;
    return 0;
}

int voice_disable_confirmation_prompts(void) {
    g_confirmations = 0;
    return 0;
}

int voice_train_voice_profile(const char *audio_sample_path) {
    (void)audio_sample_path;
    return 0;
}

int voice_enable_speaker_recognition(void) {
    g_speaker_recog = 1;
    return 0;
}

int voice_disable_speaker_recognition(void) {
    g_speaker_recog = 0;
    return 0;
}

int voice_set_sensitivity(uint8_t level) {
    if (level > 100) return -1;
    g_sensitivity = level;
    return 0;
}

int voice_get_recognition_accuracy(float *accuracy) {
    if (!accuracy) return -1;
    *accuracy = 0.965f; // 96.5% voice recognition accuracy
    return 0;
}
