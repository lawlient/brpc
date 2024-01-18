package util

import (
	"log"
	"os"
	"path/filepath"
	"strconv"
)

func ChownRecursively(root string) {
	UID, err := strconv.ParseInt(os.Getenv("SUDO_UID"), 10, 32)
	if err != nil {
		log.Fatal(err)
	}
	GID, err := strconv.ParseInt(os.Getenv("SUDO_GID"), 10, 32)
	if err != nil {
		log.Fatal(err)
	}
	err = filepath.Walk(root,
		func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return err
			}
			err = os.Chown(path, int(UID), int(GID))
			if err != nil {
				return err
			} else {
				// fmt.Printf("File ownership of %s changed.\n", path)
			}
			return nil
		})
	if err != nil {
		log.Println(err)
	}
}

func User() string {
	uid := os.Getuid()
	gid := os.Getgid()
	user := strconv.FormatInt(int64(uid), 10)
	user += ":" + strconv.FormatInt(int64(gid), 10)
	return user
}
