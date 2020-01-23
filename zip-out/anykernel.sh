# AnyKernel3 Ramdisk Mod Script
# osm0sis @ xda-developers

## AnyKernel setup
# begin properties
properties() { '
kernel.string=Simple Kernel by oipr @xda-developers
do.devicecheck=1
do.modules=0
do.cleanup=1
do.systemless=1
do.cleanuponabort=0
device.name1=beryllium
device.name2=dipper
device.name3=polaris
supported.versions=10,10.0
'; } # end properties

# shell variables
block=/dev/block/bootdevice/by-name/boot;
is_slot_device=0;
ramdisk_compression=auto;

## AnyKernel file attributes
# set permissions/ownership for included ramdisk files
set_perm_recursive 0 0 755 644 $ramdisk/*;
set_perm_recursive 0 0 750 750 $ramdisk/init* $ramdisk/sbin;

## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. tools/ak3-core.sh;


## AnyKernel file attributes
# set permissions/ownership for included ramdisk files
set_perm_recursive 0 0 755 644 $ramdisk/*;
set_perm_recursive 0 0 750 750 $ramdisk/init* $ramdisk/sbin;

## AnyKernel install
dump_boot;

ui_print "                                          "
ui_print "  _________.__               .__          "
ui_print " /   _____/|__| _____ ______ |  |   ____  "
ui_print " \_____  \ |  |/     \\____ \|  | _/ __ \ "
ui_print " /        \|  |  Y Y  \  |_> >  |_\  ___/ "
ui_print "/_______  /|__|__|_|  /   __/|____/\___  >"
ui_print "        \/          \/|__|             \/ "
ui_print "                                          "
ui_print "$compatibility_string";
ui_print "Decompressing image kernel..."
ui_print "This might take some time!"

# Set magisk policy
ui_print "Setting up magisk policy for SELinux...";
$bin/magiskpolicy --load sepolicy --save sepolicy "allow init rootfs file execute_no_trans";
$bin/magiskpolicy --load sepolicy_debug --save sepolicy_debug "allow init rootfs file execute_no_trans";

ui_print "Regenerating image kernel and installing..."
write_boot;
## end install

