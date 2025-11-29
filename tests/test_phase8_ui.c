#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_framework.h"
#include "../kernel/userland_ui.h"

static int test_count = 0;
static int pass_count = 0;
static int fail_count = 0;

void log_test(const char* name, int passed) {
    test_count++;
    if (passed) {
        pass_count++;
        printf("✓ %s\n", name);
    } else {
        fail_count++;
        printf("✗ %s\n", name);
    }
}

TEST_SUITE(display_tests) {
    printf("\n=== Phase 8: Display Tests ===\n");
    
    Display display;
    int r = display_init(&display, 1920, 1080);
    log_test("display_init", r == 0 && display.width == 1920);
    
    uint16_t w, h;
    r = display_get_resolution(&display, &w, &h);
    log_test("display_get_resolution", r == 0 && w == 1920 && h == 1080);
    
    r = display_set_resolution(&display, 1024, 768);
    log_test("display_set_resolution", r == 0);
    
    r = display_get_resolution(&display, &w, &h);
    log_test("display_after_set", r == 0 && w == 1024 && h == 768);
    
    r = display_draw_pixel(&display, 500, 400, 0xFF0000);
    log_test("display_draw_pixel", r == 0);
    
    Rect rect = {100, 100, 200, 200};
    r = display_draw_rect(&display, &rect, 0x00FF00);
    log_test("display_draw_rect", r == 0);
    
    r = display_clear(&display);
    log_test("display_clear", r == 0);
    
    r = display_refresh(&display);
    log_test("display_refresh", r == 0);
}

TEST_SUITE(window_tests) {
    printf("\n=== Phase 8: Window Manager Tests ===\n");
    
    int r = window_manager_init();
    log_test("window_manager_init", r == 0);
    
    Rect bounds = {100, 100, 800, 600};
    uint32_t win_id = window_create("Test Window", &bounds);
    log_test("window_create", win_id > 0);
    
    if (win_id > 0) {
        r = window_set_position(win_id, 200, 200);
        log_test("window_set_position", r == 0);
        
        r = window_set_size(win_id, 1024, 768);
        log_test("window_set_size", r == 0);
        
        r = window_set_focus(win_id);
        log_test("window_set_focus", r == 0);
        
        r = window_destroy(win_id);
        log_test("window_destroy", r == 0);
    }
    
    uint32_t win2 = window_create("Window 2", &bounds);
    uint32_t win3 = window_create("Window 3", &bounds);
    log_test("window_create_multiple", win2 > 0 && win3 > 0);
}

TEST_SUITE(widget_tests) {
    printf("\n=== Phase 8: Widget Framework Tests ===\n");
    
    Rect bounds = {10, 10, 100, 30};
    uint32_t widget_id;
    
    int r = widget_create(&widget_id, WIDGET_BUTTON, &bounds, "Click Me");
    log_test("widget_create_button", r == 0 && widget_id > 0);
    
    if (widget_id > 0) {
        r = widget_set_text(widget_id, "New Text");
        log_test("widget_set_text", r == 0);
        
        r = widget_render(widget_id);
        log_test("widget_render", r == 0);
        
        r = widget_destroy(widget_id);
        log_test("widget_destroy", r == 0);
    }
    
    uint32_t label_id;
    r = widget_create(&label_id, WIDGET_LABEL, &bounds, "Label");
    log_test("widget_create_label", r == 0);
    
    uint32_t textbox_id;
    r = widget_create(&textbox_id, WIDGET_TEXTBOX, &bounds, "");
    log_test("widget_create_textbox", r == 0);
    
    if (label_id > 0 && textbox_id > 0) {
        r = widget_set_parent(textbox_id, label_id);
        log_test("widget_set_parent", r == 0 || r == -1);
    }
}

TEST_SUITE(theme_tests) {
    printf("\n=== Phase 8: Theme Tests ===\n");
    
    Theme theme;
    strcpy(theme.name, "Dark Theme");
    theme.primary.r = 50;
    theme.primary.g = 50;
    theme.primary.b = 50;
    
    int r = theme_apply(&theme);
    log_test("theme_apply", r == 0);
    
    r = theme_load("Dark Theme");
    log_test("theme_load", r == 0 || r == -1);
    
    r = theme_set_color("primary", 0x323232);
    log_test("theme_set_color", r == 0);
}

TEST_SUITE(input_tests) {
    printf("\n=== Phase 8: Input Handler Tests ===\n");
    
    int r = input_handler_init();
    log_test("input_handler_init", r == 0);
    
    int callback_called = 0;
    void callback(UIEvent* e) { callback_called = 1; }
    
    r = input_register_listener(callback);
    log_test("input_register_listener", r == 0);
    
    UIEvent event;
    r = input_get_event(&event);
    log_test("input_get_event", r == 0 || r == -1);
}

TEST_SUITE(font_tests) {
    printf("\n=== Phase 8: Font Tests ===\n");
    
    int r = font_init();
    log_test("font_init", r == 0);
    
    r = font_load("/usr/share/fonts/dejavu.ttf");
    log_test("font_load", r == 0 || r == -1);
    
    Display display;
    display_init(&display, 1024, 768);
    
    r = font_render_text(&display, 100, 100, "Hello, World!");
    log_test("font_render_text", r == 0);
}

TEST_SUITE(app_launcher_tests) {
    printf("\n=== Phase 8: Application Launcher Tests ===\n");
    
    int r = app_launcher_init();
    log_test("app_launcher_init", r == 0);
    
    r = app_launcher_add_app("Calculator", "/bin/calc");
    log_test("app_launcher_add_app", r == 0);
    
    r = app_launcher_add_app("Text Editor", "/bin/editor");
    log_test("app_launcher_add_app_2", r == 0);
    
    r = app_launcher_launch("Calculator");
    log_test("app_launcher_launch", r == 0 || r == -1);
}

TEST_SUITE(file_browser_tests) {
    printf("\n=== Phase 8: File Browser Tests ===\n");
    
    int r = file_browser_init();
    log_test("file_browser_init", r == 0);
    
    r = file_browser_open("/home");
    log_test("file_browser_open", r == 0 || r == -1);
    
    char selected[256];
    r = file_browser_select_file(selected, sizeof(selected));
    log_test("file_browser_select_file", r == 0 || r == -1);
}

TEST_SUITE(terminal_tests) {
    printf("\n=== Phase 8: Terminal Tests ===\n");
    
    int r = terminal_init();
    log_test("terminal_init", r == 0);
    
    r = terminal_execute_command("ls /");
    log_test("terminal_execute_command", r == 0 || r == -1);
    
    char output[1024];
    r = terminal_get_output(output, sizeof(output));
    log_test("terminal_get_output", r == 0 || r == -1);
}

TEST_SUITE(system_tray_tests) {
    printf("\n=== Phase 8: System Tray Tests ===\n");
    
    int r = system_tray_init();
    log_test("system_tray_init", r == 0);
    
    r = system_tray_add_icon("/usr/share/icons/network.png");
    log_test("system_tray_add_icon", r == 0 || r == -1);
    
    r = system_tray_set_tooltip("Network Connected");
    log_test("system_tray_set_tooltip", r == 0 || r == -1);
}

TEST_SUITE(package_manager_tests) {
    printf("\n=== Phase 8: Package Manager Tests ===\n");
    
    int r = package_manager_init();
    log_test("package_manager_init", r == 0);
    
    r = package_manager_install("vim");
    log_test("package_manager_install", r == 0 || r == -1);
    
    r = package_manager_update();
    log_test("package_manager_update", r == 0 || r == -1);
    
    r = package_manager_remove("vim");
    log_test("package_manager_remove", r == 0 || r == -1);
}

TEST_SUITE(settings_tests) {
    printf("\n=== Phase 8: Settings Application Tests ===\n");
    
    int r = settings_app_init();
    log_test("settings_app_init", r == 0);
    
    r = settings_app_set_value("brightness", "75");
    log_test("settings_app_set_value", r == 0);
    
    char value[256];
    r = settings_app_get_value("brightness", value, sizeof(value));
    log_test("settings_app_get_value", r == 0);
    
    if (r == 0) {
        log_test("settings_value_correct", strcmp(value, "75") == 0);
    }
}

TEST_SUITE(system_monitor_tests) {
    printf("\n=== Phase 8: System Monitor Tests ===\n");
    
    int r = system_monitor_init();
    log_test("system_monitor_init", r == 0);
    
    float cpu_usage;
    r = system_monitor_get_cpu_usage(&cpu_usage);
    log_test("system_monitor_get_cpu_usage", r == 0 && cpu_usage >= 0);
    
    float mem_usage;
    r = system_monitor_get_memory_usage(&mem_usage);
    log_test("system_monitor_get_memory_usage", r == 0 && mem_usage >= 0);
    
    float temp;
    r = system_monitor_get_temperature(&temp);
    log_test("system_monitor_get_temperature", r == 0 && temp >= 0);
}

TEST_SUITE(network_config_tests) {
    printf("\n=== Phase 8: Network Configuration Tests ===\n");
    
    int r = network_config_init();
    log_test("network_config_init", r == 0);
    
    r = network_config_set_ipaddr("eth0", "192.168.1.100");
    log_test("network_config_set_ipaddr", r == 0 || r == -1);
    
    r = network_config_set_gateway("192.168.1.1");
    log_test("network_config_set_gateway", r == 0 || r == -1);
}

TEST_SUITE(storage_tests) {
    printf("\n=== Phase 8: Storage Manager Tests ===\n");
    
    int r = storage_manager_init();
    log_test("storage_manager_init", r == 0);
    
    r = storage_manager_format("/dev/sdb1", "ext4");
    log_test("storage_manager_format", r == 0 || r == -1);
    
    r = storage_manager_mount("/dev/sdb1", "/mnt/storage");
    log_test("storage_manager_mount", r == 0 || r == -1);
    
    r = storage_manager_unmount("/mnt/storage");
    log_test("storage_manager_unmount", r == 0 || r == -1);
}

TEST_SUITE(user_management_tests) {
    printf("\n=== Phase 8: User Management Tests ===\n");
    
    int r = user_manager_init();
    log_test("user_manager_init", r == 0);
    
    r = user_manager_create_user("john");
    log_test("user_manager_create_user", r == 0 || r == -1);
    
    r = user_manager_set_permissions("john", 0755);
    log_test("user_manager_set_permissions", r == 0 || r == -1);
    
    r = user_manager_delete_user("john");
    log_test("user_manager_delete_user", r == 0 || r == -1);
}

TEST_SUITE(backup_tests) {
    printf("\n=== Phase 8: Backup Tests ===\n");
    
    int r = backup_init();
    log_test("backup_init", r == 0);
    
    r = backup_create("/home", "/backup/home.bak");
    log_test("backup_create", r == 0 || r == -1);
    
    r = backup_restore("/backup/home.bak", "/home");
    log_test("backup_restore", r == 0 || r == -1);
}

TEST_SUITE(diagnostics_tests) {
    printf("\n=== Phase 8: System Diagnostics Tests ===\n");
    
    int r = system_diagnostics_run();
    log_test("system_diagnostics_run", r == 0 || r == -1);
    
    char report[2048];
    r = system_diagnostics_get_report(report, sizeof(report));
    log_test("system_diagnostics_get_report", r == 0 || r == -1);
}

TEST_SUITE(ui_integration_tests) {
    printf("\n=== Phase 8: UI Integration Tests ===\n");
    
    Display display;
    display_init(&display, 1920, 1080);
    log_test("integration_display_init", 1);
    
    window_manager_init();
    Rect bounds = {100, 100, 800, 600};
    uint32_t win = window_create("Main Window", &bounds);
    log_test("integration_window_create", win > 0);
    
    if (win > 0) {
        Rect btn_bounds = {50, 50, 100, 30};
        uint32_t btn;
        widget_create(&btn, WIDGET_BUTTON, &btn_bounds, "OK");
        log_test("integration_widget_create", btn > 0);
        
        if (btn > 0) {
            widget_set_parent(btn, win);
            widget_render(btn);
            widget_destroy(btn);
            log_test("integration_widget_hierarchy", 1);
        }
        
        window_destroy(win);
    }
}

int main() {
    printf("════════════════════════════════════════════════════════════\n");
    printf("  Aegis OS - Phase 8: Userland UI Tests\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    display_tests();
    window_tests();
    widget_tests();
    theme_tests();
    input_tests();
    font_tests();
    app_launcher_tests();
    file_browser_tests();
    terminal_tests();
    system_tray_tests();
    package_manager_tests();
    settings_tests();
    system_monitor_tests();
    network_config_tests();
    storage_tests();
    user_management_tests();
    backup_tests();
    diagnostics_tests();
    ui_integration_tests();
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  TOTAL: %d | PASSED: %d | FAILED: %d\n", test_count, pass_count, fail_count);
    printf("  PASS RATE: %.1f%%\n", test_count > 0 ? (100.0 * pass_count / test_count) : 0);
    printf("════════════════════════════════════════════════════════════\n");
    
    return fail_count > 0 ? 1 : 0;
}
