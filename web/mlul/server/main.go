package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	DBInit()
	r := gin.Default()

	r.POST("/register", Register)
	r.POST("/login", Login)

	api := r.Group("/api/")
	api.Use(JWTMiddleware())
	{
		api.GET("/name", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"code": http.StatusOK,
				"name": "xujiuan",
			})
		})
	}

	r.GET("/", func(c *gin.Context) {
		c.Redirect(http.StatusMovedPermanently, "/mlul")
	})
	r.StaticFS("/mlul", http.Dir("public/"))
	r.StaticFS("/music", http.Dir("music/"))
	r.Run(":12827")
}
