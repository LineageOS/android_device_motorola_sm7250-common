#!/vendor/bin/sh


FIRST_NEW_BOOTLOADER_DATE=`getprop ro.vendor.new_bootloader`
CURRENT_BOOTLOADER_DATE=`getprop ro.boot.bootloader | sed "s/.*-//"`

if [ -z $FIRST_NEW_BOOTLOADER_DATE ]; then
    # No hack needed
    setprop ro.vendor.load.ipa "1"
    exit
fi

if [ $CURRENT_BOOTLOADER_DATE -lt $FIRST_NEW_BOOTLOADER_DATE ]; then
    echo "Its OLD bootloader"
    # do mountings
    setprop ro.vendor.load.old_ipa "1"
else
    setprop ro.vendor.load.ipa "1"
fi
