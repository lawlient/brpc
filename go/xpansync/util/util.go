package util

import (
	"crypto/md5"
	"fmt"
	"os/exec"
	"strconv"
)

func Byte2IEC(b int64) string {
	const unit = 1024
	if b < unit {
		return fmt.Sprintf("%d B", b)
	}

	div, exp := int64(unit), 0
	for n := b / unit; n >= unit; n /= unit {
		div *= unit
		exp++
	}

	return fmt.Sprintf("%.1f %cB", float64(b)/float64(div), "KMGTPE"[exp])
}

func Md5String(data []byte) string {
	hash := md5.Sum(data)
	md5str := fmt.Sprintf("%x", hash)
	return md5str
}

func Itoa(num int) string {
	return strconv.Itoa(num)
}

func Atoi(num string) int {
	i, err := strconv.Atoi(num)
	if err != nil {
		return 0
	}
	return i
}

func TargzPackage(dsc, src string) error {
	cmd := exec.Command("tar", "-zcf", dsc, src)
	out, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Println(out)
		return err
	}
	return nil
}
