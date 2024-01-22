package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

func Login(c *gin.Context) {
	var user User
	if err := c.ShouldBindJSON(&user); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"success": false,
			"msg":     "请输入用户名和密码",
		})
		return
	}

	var u User
	u.Name = user.Name
	if !GetUser(&u) {
		c.JSON(http.StatusNotFound, gin.H{
			"success": false,
			"msg":     "用户不存在",
		})
		return
	}
	// compare hash pwd & plain pwd
	if err := HashPasswdCheck(u.Password, user.Password); err != nil {
		c.JSON(http.StatusForbidden, gin.H{
			"success": false,
			"msg":     "密码错误",
		})
		return
	}

	token, _ := SignJWT(user)
	c.JSON(http.StatusOK, gin.H{
		"success": true,
		"msg":     "登录成功",
		"data": gin.H{
			"user":  user.Name,
			"token": token,
		},
	})
}

func Register(c *gin.Context) {
	var user User
	if err := c.ShouldBindJSON(&user); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"success": false,
			"msg":     "请输入用户名和密码",
		})
		return
	}

	// Check if name already exists
	var u User
	u.Name = user.Name

	if GetUser(&u) {
		c.JSON(http.StatusConflict, gin.H{
			"success": false,
			"msg":     "用户名已存在",
		})
		return
	}

	// Insert new user
	err := AddUser(user)
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"success": false,
			"msg":     "数据库访问失败",
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"success": true,
		"msg":     "注册成功",
	})
	return
}
