#!/bin/sh
# shellcheck disable=SC2086 # FIXME: fix these globing warnings

set -e

die() {
    echo "die: $*"
    exit 1
}

#THEOS_PACKET_LOGGING_ARG="-object filter-dump,id=hue,netdev=breh,file=e1000.pcap"

[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && THEOS_VIRT_TECH_ARG="-enable-kvm"

[ -z "$THEOS_BOCHS_BIN" ] && THEOS_BOCHS_BIN="bochs"

# To support virtualization acceleration on mac
# we need to use 64-bit qemu
if [ "$(uname)" = "Darwin" ] && [ "$(uname -m)" = "x86_64" ]; then

    [ -z "$THEOS_QEMU_BIN" ] && THEOS_QEMU_BIN="qemu-system-x86_64"

    if $THEOS_QEMU_BIN --accel help | grep -q hvf; then
        THEOS_VIRT_TECH_ARG="--accel hvf"
    fi
fi

THEOS_RUN="${THEOS_RUN:-$1}"

[ -z "$THEOS_QEMU_BIN" ] && THEOS_QEMU_BIN="qemu-system-i386"

[ -z "$THEOS_KERNEL_CMDLINE" ] && THEOS_KERNEL_CMDLINE="hello"

[ -z "$THEOS_RAM_SIZE" ] && THEOS_RAM_SIZE=512M

[ -z "$THEOS_QEMU_CPU" ] && THEOS_QEMU_CPU="max"

[ -z "$THEOS_DISK_IMAGE" ] && {
    if [ "$THEOS_RUN" = qgrub ]; then
        THEOS_DISK_IMAGE="grub_disk_image"
    else
        THEOS_DISK_IMAGE="_disk_image"
    fi
}

[ -z "$THEOS_COMMON_QEMU_ARGS" ] && THEOS_COMMON_QEMU_ARGS="
$THEOS_EXTRA_QEMU_ARGS
-s -m $THEOS_RAM_SIZE
-cpu $THEOS_QEMU_CPU
-d guest_errors
-smp 2
-device VGA,vgamem_mb=64
-drive file=${THEOS_DISK_IMAGE},format=raw,index=0,media=disk
-device ich9-ahci
-usb
-debugcon stdio
-soundhw pcspk
-device sb16
"

[ -z "$THEOS_COMMON_QEMU_Q35_ARGS" ] && THEOS_COMMON_QEMU_Q35_ARGS="
$THEOS_EXTRA_QEMU_ARGS
-s -m $THEOS_RAM_SIZE
-cpu $THEOS_QEMU_CPU
-machine q35
-d guest_errors
-smp 2
-device VGA,vgamem_mb=64
-drive file=${THEOS_DISK_IMAGE},id=disk,if=none
-device ahci,id=ahci
-device ide-hd,bus=ahci.0,drive=disk,unit=0
-usb
-debugcon stdio
-soundhw pcspk
-device sb16
"

export SDL_VIDEO_X11_DGAMOUSE=0

: "${THEOS_BUILD:=.}"
cd -P -- "$THEOS_BUILD" || die "Could not cd to \"$THEOS_BUILD\""

if [ "$THEOS_RUN" = "b" ]; then
    # Meta/run.sh b: bochs
    [ -z "$THEOS_BOCHSRC" ] && {
        # Make sure that THEOS_ROOT is set and not empty
        [ -z "$THEOS_ROOT" ] && die 'THEOS_ROOT not set or empty'
        THEOS_BOCHSRC="$THEOS_ROOT/Meta/bochsrc"
    }
    "$THEOS_BOCHS_BIN" -q -f "$THEOS_BOCHSRC"
elif [ "$THEOS_RUN" = "qn" ]; then
    # Meta/run.sh qn: qemu without network
    "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_ARGS \
        -device e1000 \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
elif [ "$THEOS_RUN" = "qtap" ]; then
    # Meta/run.sh qtap: qemu with tap
    sudo "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_ARGS \
        $THEOS_VIRT_TECH_ARG \
        $THEOS_PACKET_LOGGING_ARG \
        -netdev tap,ifname=tap0,id=br0 \
        -device e1000,netdev=br0 \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
elif [ "$THEOS_RUN" = "qgrub" ]; then
    # Meta/run.sh qgrub: qemu with grub
    "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_ARGS \
        $THEOS_VIRT_TECH_ARG \
        $THEOS_PACKET_LOGGING_ARG \
        -netdev user,id=breh,hostfwd=tcp:127.0.0.1:8888-10.0.2.15:8888,hostfwd=tcp:127.0.0.1:8823-10.0.2.15:23 \
        -device e1000,netdev=breh
elif [ "$THEOS_RUN" = "q35_cmd" ]; then
    # Meta/run.sh q35_cmd: qemu (q35 chipset) with TheoOS with custom commandline
    shift
    THEOS_KERNEL_CMDLINE="$*"
    echo "Starting TheoOS, Commandline: ${THEOS_KERNEL_CMDLINE}"
    "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_Q35_ARGS \
        $THEOS_VIRT_TECH_ARG \
        -netdev user,id=breh,hostfwd=tcp:127.0.0.1:8888-10.0.2.15:8888,hostfwd=tcp:127.0.0.1:8823-10.0.2.15:23 \
        -device e1000,netdev=breh \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
elif [ "$THEOS_RUN" = "qcmd" ]; then
    # Meta/run.sh qcmd: qemu with TheoOS with custom commandline
    shift
    THEOS_KERNEL_CMDLINE="$*"
    echo "Starting TheOS, Commandline: ${THEOS_KERNEL_CMDLINE}"
    "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_ARGS \
        $THEOS_VIRT_TECH_ARG \
        -netdev user,id=breh,hostfwd=tcp:127.0.0.1:8888-10.0.2.15:8888,hostfwd=tcp:127.0.0.1:8823-10.0.2.15:23 \
        -device e1000,netdev=breh \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
elif [ "$THEOS_RUN" = "ci" ]; then
    # Meta/run.sh ci: qemu in text mode
    echo "Running QEMU in CI"
    "$THEOS_QEMU_BIN" \
        $THEOS_EXTRA_QEMU_ARGS \
        -s -m $THEOS_RAM_SIZE \
        -cpu $THEOS_QEMU_CPU \
        -d guest_errors \
        -smp 2 \
        -drive file=${THEOS_DISK_IMAGE},format=raw,index=0,media=disk \
        -device ich9-ahci \
        -nographic \
        -display none \
        -debugcon file:debug.log \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
else
    # Meta/run.sh: qemu with user networking
    "$THEOS_QEMU_BIN" \
        $THEOS_COMMON_QEMU_ARGS \
        $THEOS_VIRT_TECH_ARG \
        $THEOS_PACKET_LOGGING_ARG \
        -netdev user,id=breh,hostfwd=tcp:127.0.0.1:8888-10.0.2.15:8888,hostfwd=tcp:127.0.0.1:8823-10.0.2.15:23,hostfwd=tcp:127.0.0.1:8000-10.0.2.15:8000,hostfwd=tcp:127.0.0.1:2222-10.0.2.15:22 \
        -device e1000,netdev=breh \
        -kernel Kernel/Kernel \
        -append "${THEOS_KERNEL_CMDLINE}"
fi