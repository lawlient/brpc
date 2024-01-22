package main

import (
	"net/http"

	"time"

	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt"
)

var salt = []byte("music_light_up_life")

func SignJWT(user User) (string, error) {
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.MapClaims{
		"name":     user.Name,
		"password": user.Password,
		"expire":   time.Now().Add(time.Hour * 6).Unix(),
	})
	tokenStr, err := token.SignedString(salt)
	return tokenStr, err
}

func ParseJWT(tokenStr string) (*jwt.Token, error) {
	token, err := jwt.Parse(tokenStr, func(token *jwt.Token) (i interface{}, e error) {
		return salt, nil
	})
	return token, err
}

func JWTMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		reqToken := c.Request.Header.Get("Authorization")
		token, err := ParseJWT(reqToken)
		if err != nil {
			goto fail
		}

		if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
			c.Set("username", claims["name"])
			c.Set("uid", claims["id"])
			c.Next()
			return
		} else {
			goto fail
		}

	fail:
		c.JSON(http.StatusUnauthorized, gin.H{
			"success": false,
			"msg":     "未登录",
		})
		c.Abort()
		return

	}
}
