package util

import (
	"crypto/md5"
	"fmt"
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
