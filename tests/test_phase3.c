#include "../kernel/device_tree.h"
#include "../kernel/devfs.h"
#include "../kernel/sysfs.h"
#include "../kernel/multiboot2.h"
#include "../kernel/arm_boot.h"
#include "../kernel/boot_handoff.h"
#include "test_framework.h"
#include <string.h>
#include <stdlib.h>

TEST_SUITE("Phase 3: Device Nodes & Bootloaders");

TEST(device_tree_parse_basic) {
    uint32_t dtb_data[32];
    memset(dtb_data, 0, sizeof(dtb_data));
    dtb_data[0] = 0xd00dfeed;
    dtb_data[1] = 128;
    dtb_data[2] = 0x00000008;
    dtb_data[3] = 0x00000040;
    
    dt_tree_t *tree = dt_parse_blob(dtb_data, 128);
    
    ASSERT_NOT_NULL(tree);
    ASSERT_NOT_NULL(tree->root);
    ASSERT_EQUAL(tree->node_count, 1);
    
    dt_free_tree(tree);
    PASS();
}

TEST(device_tree_create_nodes) {
    dt_tree_t *tree = malloc(sizeof(dt_tree_t));
    memset(tree, 0, sizeof(dt_tree_t));
    tree->nodes = malloc(sizeof(dt_node_t *) * 512);
    tree->string_buffer = malloc(65536);
    tree->phandle_map = malloc(sizeof(uint32_t) * 1024);
    
    dt_node_t *root = dt_create_node(tree, "root");
    ASSERT_NOT_NULL(root);
    ASSERT_EQUAL(tree->node_count, 1);
    
    dt_node_t *child = dt_create_node(tree, "cpus");
    ASSERT_NOT_NULL(child);
    ASSERT_EQUAL(tree->node_count, 2);
    
    dt_free_tree(tree);
    PASS();
}

TEST(device_tree_properties) {
    dt_tree_t *tree = malloc(sizeof(dt_tree_t));
    memset(tree, 0, sizeof(dt_tree_t));
    tree->nodes = malloc(sizeof(dt_node_t *) * 512);
    tree->string_buffer = malloc(65536);
    tree->phandle_map = malloc(sizeof(uint32_t) * 1024);
    
    dt_node_t *node = dt_create_node(tree, "test");
    ASSERT_NOT_NULL(node);
    
    uint32_t test_value = 0x12345678;
    bool added = dt_add_property(node, "test-prop", &test_value, sizeof(uint32_t));
    ASSERT_TRUE(added);
    ASSERT_EQUAL(node->property_count, 1);
    
    dt_property_t *prop = dt_get_property(node, "test-prop");
    ASSERT_NOT_NULL(prop);
    
    dt_free_tree(tree);
    PASS();
}

TEST(devfs_init) {
    devfs_t *devfs = devfs_init();
    
    ASSERT_NOT_NULL(devfs);
    ASSERT_EQUAL(devfs->device_count, 0);
    ASSERT_NOT_NULL(devfs->devices);
    ASSERT_NOT_NULL(devfs->ops);
    
    devfs_free(devfs);
    PASS();
}

TEST(devfs_register_char_device) {
    devfs_t *devfs = devfs_init();
    ASSERT_NOT_NULL(devfs);
    
    devfs_operations_t ops = {0};
    int major = devfs_register_char_device(devfs, "test0", 1, 0, &ops);
    
    ASSERT_GREATER_EQUAL(major, 0);
    ASSERT_EQUAL(devfs->device_count, 1);
    
    devfs_device_t *dev = devfs_get_device(devfs, "test0");
    ASSERT_NOT_NULL(dev);
    ASSERT_EQUAL(dev->major, 1);
    ASSERT_EQUAL(dev->type, DEVFS_TYPE_CHAR);
    
    devfs_free(devfs);
    PASS();
}

TEST(devfs_register_block_device) {
    devfs_t *devfs = devfs_init();
    ASSERT_NOT_NULL(devfs);
    
    devfs_operations_t ops = {0};
    int major = devfs_register_block_device(devfs, "sda", 8, 0, 1000000, &ops);
    
    ASSERT_GREATER_EQUAL(major, 0);
    ASSERT_EQUAL(devfs->device_count, 1);
    
    devfs_device_t *dev = devfs_get_device(devfs, "sda");
    ASSERT_NOT_NULL(dev);
    ASSERT_EQUAL(dev->type, DEVFS_TYPE_BLOCK);
    ASSERT_EQUAL(dev->size, 1000000);
    
    devfs_free(devfs);
    PASS();
}

TEST(devfs_enumerate_devices) {
    devfs_t *devfs = devfs_init();
    ASSERT_NOT_NULL(devfs);
    
    devfs_operations_t ops = {0};
    devfs_register_char_device(devfs, "dev1", 1, 0, &ops);
    devfs_register_char_device(devfs, "dev2", 2, 0, &ops);
    devfs_register_char_device(devfs, "dev3", 3, 0, &ops);
    
    devfs_device_t *devices[10];
    int count = devfs_enumerate_devices(devfs, devices, 10);
    
    ASSERT_EQUAL(count, 3);
    
    devfs_free(devfs);
    PASS();
}

TEST(devfs_hotplug) {
    devfs_t *devfs = devfs_init();
    ASSERT_NOT_NULL(devfs);
    
    devfs_operations_t ops = {0};
    devfs_register_char_device(devfs, "hotdev", 1, 0, &ops);
    devfs_device_t *dev = devfs_get_device(devfs, "hotdev");
    
    ASSERT_FALSE(dev->hotplug);
    bool enabled = devfs_enable_hotplug(dev);
    ASSERT_TRUE(enabled);
    ASSERT_TRUE(dev->hotplug);
    
    bool disabled = devfs_disable_hotplug(dev);
    ASSERT_TRUE(disabled);
    ASSERT_FALSE(dev->hotplug);
    
    devfs_free(devfs);
    PASS();
}

TEST(sysfs_init) {
    sysfs_t *sysfs = sysfs_init();
    
    ASSERT_NOT_NULL(sysfs);
    ASSERT_NOT_NULL(sysfs->root);
    ASSERT_EQUAL(sysfs->entry_count, 1);
    ASSERT_EQUAL(strcmp(sysfs->root->name, "/"), 0);
    
    sysfs_free(sysfs);
    PASS();
}

TEST(sysfs_create_directories) {
    sysfs_t *sysfs = sysfs_init();
    ASSERT_NOT_NULL(sysfs);
    
    sysfs_entry_t *sys_dir = sysfs_create_dir(sysfs, "sys", sysfs->root);
    ASSERT_NOT_NULL(sys_dir);
    ASSERT_EQUAL(sys_dir->type, SYSFS_TYPE_DIR);
    ASSERT_EQUAL(sysfs->entry_count, 2);
    
    sysfs_entry_t *kernel_dir = sysfs_create_dir(sysfs, "kernel", sys_dir);
    ASSERT_NOT_NULL(kernel_dir);
    ASSERT_EQUAL(sysfs->entry_count, 3);
    
    sysfs_free(sysfs);
    PASS();
}

TEST(sysfs_create_attributes) {
    sysfs_t *sysfs = sysfs_init();
    ASSERT_NOT_NULL(sysfs);
    
    const char *test_value = "test_value";
    sysfs_entry_t *attr = sysfs_create_attr(sysfs, "attr1", sysfs->root,
                                            test_value, strlen(test_value) + 1);
    
    ASSERT_NOT_NULL(attr);
    ASSERT_EQUAL(attr->type, SYSFS_TYPE_ATTR);
    ASSERT_EQUAL(attr->value_size, strlen(test_value) + 1);
    
    char buffer[256];
    int size = sysfs_read_attr(attr, buffer, 256);
    ASSERT_GREATER(size, 0);
    
    sysfs_free(sysfs);
    PASS();
}

TEST(sysfs_find_entry) {
    sysfs_t *sysfs = sysfs_init();
    ASSERT_NOT_NULL(sysfs);
    
    sysfs_entry_t *sys_dir = sysfs_create_dir(sysfs, "sys", sysfs->root);
    sysfs_entry_t *kernel_dir = sysfs_create_dir(sysfs, "kernel", sys_dir);
    
    sysfs_entry_t *found = sysfs_find_entry(sysfs, "/sys/kernel");
    ASSERT_NOT_NULL(found);
    ASSERT_EQUAL(found, kernel_dir);
    
    sysfs_free(sysfs);
    PASS();
}

TEST(multiboot2_validation) {
    uint32_t fake_mbi[16];
    memset(fake_mbi, 0, sizeof(fake_mbi));
    
    bool valid = multiboot2_is_valid(fake_mbi, 0x2badb002);
    ASSERT_FALSE(valid);
    
    PASS();
}

TEST(boot_handoff_create) {
    boot_handoff_t *handoff = boot_create_handoff();
    
    ASSERT_NOT_NULL(handoff);
    ASSERT_EQUAL(handoff->magic, BOOT_HANDOFF_MAGIC);
    ASSERT_EQUAL(handoff->version, BOOT_HANDOFF_VERSION);
    
    boot_free_handoff(handoff);
    PASS();
}

TEST(boot_handoff_add_memory_region) {
    boot_handoff_t *handoff = boot_create_handoff();
    ASSERT_NOT_NULL(handoff);
    
    int idx = boot_add_memory_region(handoff, 0x00000000, 0x100000, 1);
    ASSERT_EQUAL(idx, 0);
    ASSERT_EQUAL(handoff->memory_region_count, 1);
    
    idx = boot_add_memory_region(handoff, 0x01000000, 0x100000, 1);
    ASSERT_EQUAL(idx, 1);
    ASSERT_EQUAL(handoff->memory_region_count, 2);
    
    boot_free_handoff(handoff);
    PASS();
}

TEST(boot_handoff_add_modules) {
    boot_handoff_t *handoff = boot_create_handoff();
    ASSERT_NOT_NULL(handoff);
    
    int idx = boot_add_module(handoff, 0x10000000, 0x100000, "initrd", "");
    ASSERT_EQUAL(idx, 0);
    ASSERT_EQUAL(handoff->boot_module_count, 1);
    
    boot_module_t *mod = &handoff->boot_modules[0];
    ASSERT_EQUAL(mod->address, 0x10000000);
    ASSERT_EQUAL(mod->size, 0x100000);
    
    boot_free_handoff(handoff);
    PASS();
}

TEST(boot_handoff_framebuffer) {
    boot_handoff_t *handoff = boot_create_handoff();
    ASSERT_NOT_NULL(handoff);
    
    ASSERT_FALSE(boot_has_framebuffer(handoff));
    
    int result = boot_set_framebuffer_info(handoff, 0xA0000, 1024, 768, 4096, 32);
    ASSERT_EQUAL(result, 0);
    ASSERT_TRUE(boot_has_framebuffer(handoff));
    ASSERT_EQUAL(handoff->framebuffer.width, 1024);
    ASSERT_EQUAL(handoff->framebuffer.height, 768);
    
    boot_free_handoff(handoff);
    PASS();
}

TEST(boot_handoff_acpi) {
    boot_handoff_t *handoff = boot_create_handoff();
    ASSERT_NOT_NULL(handoff);
    
    ASSERT_FALSE(boot_has_acpi(handoff));
    
    int result = boot_set_acpi_info(handoff, 0xF0000, 2);
    ASSERT_EQUAL(result, 0);
    ASSERT_TRUE(boot_has_acpi(handoff));
    
    boot_free_handoff(handoff);
    PASS();
}

TEST(arm_boot_info_create) {
    arm_boot_info_t *info = arm_init_boot_info();
    
    ASSERT_NOT_NULL(info);
    ASSERT_EQUAL(info->mode, ARM_BOOT_MODE_32BIT);
    ASSERT_EQUAL(info->cpu_id, 0);
    
    free(info);
    PASS();
}

TEST(arm_device_tree_validation) {
    uint32_t dtb[32];
    memset(dtb, 0, sizeof(dtb));
    
    dtb[0] = 0xd00dfeed;
    dtb[1] = 128;
    
    bool valid = arm_validate_device_tree(dtb, 128);
    ASSERT_TRUE(valid);
    
    bool invalid = arm_validate_device_tree(dtb, 10);
    ASSERT_FALSE(invalid);
    
    PASS();
}

TEST(device_tree_parser_load) {
    uint8_t dtb_buffer[256];
    memset(dtb_buffer, 0, sizeof(dtb_buffer));
    
    uint32_t *header = (uint32_t *)dtb_buffer;
    header[0] = 0xd00dfeed;
    header[1] = 0x00000100;
    header[2] = 0x00000008;
    header[3] = 0x00000100;
    header[4] = 0x00000100;
    header[5] = 0x00000011;
    header[6] = 0x00000010;
    header[7] = 0x00000000;
    header[8] = 0x00000000;
    header[9] = 0x00000100;
    
    dt_tree_t *tree = dt_parse_blob(dtb_buffer, 256);
    
    if (tree) {
        ASSERT_NOT_NULL(tree->root);
        dt_free_tree(tree);
    }
    
    PASS();
}

TEST(devfs_get_devices_by_type) {
    devfs_t *devfs = devfs_init();
    ASSERT_NOT_NULL(devfs);
    
    devfs_operations_t ops = {0};
    devfs_register_char_device(devfs, "tty0", 4, 0, &ops);
    devfs_register_char_device(devfs, "tty1", 4, 1, &ops);
    devfs_register_block_device(devfs, "sda", 8, 0, 1000000, &ops);
    
    size_t count = 0;
    devfs_device_t **char_devices = devfs_get_devices_by_type(devfs, DEVFS_TYPE_CHAR, &count);
    
    ASSERT_NOT_NULL(char_devices);
    ASSERT_EQUAL(count, 2);
    
    free(char_devices);
    devfs_free(devfs);
    PASS();
}

TEST(sysfs_observer_callbacks) {
    sysfs_t *sysfs = sysfs_init();
    ASSERT_NOT_NULL(sysfs);
    
    sysfs_entry_t *attr = sysfs_create_attr(sysfs, "test_attr", sysfs->root, "initial", 8);
    ASSERT_NOT_NULL(attr);
    
    ASSERT_FALSE(sysfs_register_observer(NULL, NULL));
    ASSERT_TRUE(sysfs_register_observer(attr, NULL));
    
    sysfs_free(sysfs);
    PASS();
}

TEST(boot_handoff_validation) {
    boot_handoff_t *handoff = boot_create_handoff();
    ASSERT_NOT_NULL(handoff);
    
    boot_add_memory_region(handoff, 0x00000000, 0x100000, 1);
    
    boot_status_t status = boot_validate_handoff(handoff);
    ASSERT_EQUAL(status, BOOT_STATUS_SUCCESS);
    
    boot_free_handoff(handoff);
    PASS();
}

int main(int argc, char *argv[]) {
    RUN_ALL_TESTS();
    return TEST_RESULTS_OK() ? 0 : 1;
}
