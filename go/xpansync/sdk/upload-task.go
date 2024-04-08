package sdk

import (
	"net/http"

	"xpansync/db"

	"github.com/gin-gonic/gin"
)

func AddTask(c *gin.Context) {
	var t db.Task
	if err := c.ShouldBindJSON(&t); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{})
		return
	}

	err := db.INS.UpsertTask(&t)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{})
}

func DelTask(c *gin.Context) {
	var t db.Task
	if err := c.ShouldBindJSON(&t); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{})
		return
	}

	err := db.INS.DelTask(&t)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}
	c.JSON(http.StatusOK, gin.H{})
}

func GetAllTask(c *gin.Context) {
	tasks, err := db.INS.GetAllTask()
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"data": tasks,
	})
}

func UpdateTask(c *gin.Context) {
	var t db.Task
	if err := c.ShouldBindJSON(&t); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{})
		return
	}
	if t.ID == 0 {
		c.JSON(http.StatusBadRequest, gin.H{
			"msg": "ID is required",
		})
		return
	}

	err := db.INS.UpsertTask(&t)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{
			"msg": err.Error(),
		})
		return
	}
	c.JSON(http.StatusOK, gin.H{})
}
