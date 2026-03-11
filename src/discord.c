#include "dsp_renderer.h"
#include <stdio.h>
#include <string.h>

#ifdef Discord

/* Define states */
struct IDiscordCore* g_discord_core = NULL;
struct IDiscordActivityManager* g_activity_manager = NULL;

void discord_start() {
    struct DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    
    params.client_id = 1481356778007429287LL; 
    params.flags = DiscordCreateFlags_NoRequireDiscord;

    enum EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &g_discord_core);

    if (result != DiscordResult_Ok) {
        printf("Discord: Failed to initialize (Error %d)\n", result);
        return;
    }

    g_activity_manager = g_discord_core->get_activity_manager(g_discord_core);
    
    struct DiscordActivity activity;
    memset(&activity, 0, sizeof(activity));
    
    /* Set fields, temporarily */
    strncpy(activity.details, "0.0.11a", sizeof(activity.details) - 1);
    
    /* Set icon and title */
    strncpy(activity.assets.large_image, "icon", sizeof(activity.assets.large_image) - 1);
    strncpy(activity.assets.large_text, "Classica", sizeof(activity.assets.large_text) - 1);

    g_activity_manager->update_activity(g_activity_manager, &activity, NULL, NULL);
    printf("Discord: Rich Presence initialized for Classica.\n");
}

void discord_update(char *text, char *textSmall) {
    if (g_discord_core) {
        /* Run callbacks to process the activity update and events */
        g_discord_core->run_callbacks(g_discord_core);
        g_activity_manager = g_discord_core->get_activity_manager(g_discord_core);
    
        struct DiscordActivity activity;
        memset(&activity, 0, sizeof(activity));
        
        /* Set the fields */
        strncpy(activity.details, text, sizeof(activity.details) - 1);
        strncpy(activity.state, textSmall, sizeof(activity.state) - 1);
        
        /* Set icon and title */
        strncpy(activity.assets.large_image, "icon", sizeof(activity.assets.large_image) - 1);
        strncpy(activity.assets.large_text, "Classica", sizeof(activity.assets.large_text) - 1);

        g_activity_manager->update_activity(g_activity_manager, &activity, NULL, NULL);
    }
}

void discord_end() {
    if (g_discord_core) {
        /* Clean up */
        g_discord_core->destroy(g_discord_core);
        g_discord_core = NULL;
        g_activity_manager = NULL;
    }
}
#endif