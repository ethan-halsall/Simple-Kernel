# AnyKernel2 Ramdisk Mod Script
# osm0sis @ xda-developers

## AnyKernel setup
# begin properties
properties() {'
kernel.string=Simple Kernel by oipr @xda-developers
do.devicecheck=1
do.modules=0
do.cleanup=1
do.cleanuponabort=0
device.name1=beryllium
device.name2=dipper
device.name3=polaris
supported.versions=9,9.0
'} # end properties

# shell variables
block=/dev/block/bootdevice/by-name/boot;
is_slot_device=auto;
ramdisk_compression=auto;


## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. /tmp/anykernel/tools/ak2-core.sh;

## AnyKernel file attributes
# set permissions/ownership for included ramdisk files
chmod -R 750 $ramdisk/*;
chown -R root:root $ramdisk/*;

LD_PATH=/system/lib
if [ -d /system/lib64 ]; then
  LD_PATH=/system/lib64
fi

exec_util() {
  LD_LIBRARY_PATH=/system/lib64 $UTILS $1
}

set_con() {
  exec_util "chcon -h u:object_r:"$1":s0 $2"
  exec_util "chcon u:object_r:"$1":s0 $2"
}

ui_print "                                          "
ui_print "  _________.__               .__          "
ui_print " /   _____/|__| _____ ______ |  |   ____  "
ui_print " \_____  \ |  |/     \\____ \|  | _/ __ \ "
ui_print " /        \|  |  Y Y  \  |_> >  |_\  ___/ "
ui_print "/_______  /|__|__|_|  /   __/|____/\___  >"
ui_print "        \/          \/|__|             \/ "
ui_print "                                          "
ui_print "$compatibility_string";

## Trim partitions
#ui_print " "
#ui_print "Triming cache & data partitions..."
#fstrim -v /cache;
#fstrim -v /data;


ui_print "Decompressing image kernel..."
ui_print "This might take some seconds."
## AnyKernel install
dump_boot;

# ramdisk patch

umount /vendor || true
mount -o rw /dev/block/bootdevice/by-name/vendor /vendor
exec_util "cp -a /tmp/anykernel/ramdisk/init.simple.sh /vendor/bin/"
set_con qti_init_shell_exec /vendor/bin/init.simple.sh
umount /vendor || true
# end ramdisk changes

# Set magisk policy
ui_print "Setting up magisk policy for SELinux...";
$bin/magiskpolicy --load sepolicy --save sepolicy "allow init rootfs file execute_no_trans";
$bin/magiskpolicy --load sepolicy_debug --save sepolicy_debug "allow init rootfs file execute_no_trans";

ui_print "Regenerating image kernel and installing..."
write_boot;

## end install

