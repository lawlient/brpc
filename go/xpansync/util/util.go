package util

import (
	"crypto/md5"
	"fmt"
	"os/exec"
	"strconv"
	"time"

	"github.com/golang-jwt/jwt/v4"
	"github.com/pkg/errors"
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

const (
	Issuer       = "xpansync"
	AudienceName = "access-token"
	TokenExpire  = 86400 * time.Second
	Secret       = "xpansync is good assistant"
)

type ClaimsMessage struct {
	Name string `json:"name"`
	jwt.RegisteredClaims
}

func GenerateAccessToken(username string, uid int, exp time.Time, secret []byte) (string, error) {
	claims := jwt.RegisteredClaims{
		Issuer:   Issuer,
		Audience: jwt.ClaimStrings{AudienceName},
		IssuedAt: jwt.NewNumericDate(time.Now()),
		Subject:  fmt.Sprint(uid),
	}

	if !exp.IsZero() {
		claims.ExpiresAt = jwt.NewNumericDate(exp)
	}

	token := jwt.NewWithClaims(jwt.SigningMethodHS256, &ClaimsMessage{
		Name:             username,
		RegisteredClaims: claims,
	})

	tokenstr, err := token.SignedString(secret)
	if err != nil {
		return "", err
	}

	return tokenstr, nil
}

func GetUidFromAccessToken(accessToken, secret string) (int, error) {
	claims := &ClaimsMessage{}
	_, err := jwt.ParseWithClaims(accessToken, claims, func(t *jwt.Token) (any, error) {
		if t.Method.Alg() != jwt.SigningMethodHS256.Name {
			return nil, errors.Errorf("unexpected access token signing method=%v, expect %v", t.Header["alg"], jwt.SigningMethodHS256)
		}
		return []byte(secret), nil
	})
	if err != nil {
		return 0, errors.Wrap(err, "Invalid or expired access token")
	}
	uid, err := strconv.Atoi(claims.Subject)
	if err != nil {
		return 0, errors.Wrap(err, "Malformed ID in the token")
	}
	return int(uid), nil
}
