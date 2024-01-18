package config

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"reflect"
)

type Config struct {
	Project      string `json:"project"`
	Output       string `json:"output_dir"`
	Port         int32  `json:"port"`
	Pm           string `json:"pm"`
	Template     string `json:"template"`
	FrontendImg  string `json:"frontend_image"`
	BackendImg   string `json:"backend_image"`
	FrontendPort int32  `json:"frontend_port"`
	BackendPort  int32  `json:"backend_port"`
	Tailwindcss  bool   `json:"tailwindcss"`
	UI           string `json:"ui"`
}

func (config *Config) Parse(name string) {
	file, err := os.Open(name)
	if err != nil {
		log.Fatalln(err)
	}

	data, _ := ioutil.ReadAll(file)
	json.Unmarshal(data, config)

	config.Show()
}

func (config *Config) Root() string {
	return config.Output + "/" + config.Project
}

func (config *Config) Show() {
	/// println("Project Name is ", config.Project)
	/// println("root directory is ", config.Output)
	/// println("server port is ", config.Port)
	/// println("package manager is ", config.Pm)
	/// println("template is ", config.Template)
	/// println("frontend image is ", config.FrontendImg)
	/// println("backend image is ", config.BackendImg)

	v := reflect.ValueOf(*config)
	values := make([]interface{}, v.NumField())
	for i := 0; i < v.NumField(); i++ {
		values[i] = v.Field(i).Interface()
	}

	fmt.Println(values)
}
