/bin/time -v ./lzss_MayMun.elf C "$1" "$1".MM
echo
/bin/time -v ./lzss_MayMun.elf e "$1".MM "$1".restore
echo
b3sum "$1"
b3sum "$1".restore


