package db

import "fmt"

type Store struct {
	DB
}

func NewStore() (*Store, error) {
	store, err := InitDB("sqlite://" + "/var/lib/xpansync/" + "setting.db")
	if err != nil {
		fmt.Println(err)
		return nil, err
	}
	return &Store{DB: store}, nil
}

func (s *Store) getsetting(key string) string {
	value, err := s.GetSetting(key)
	if err != nil {
		return ""
	}
	return *value

}

func (s *Store) GetToken() string { return s.getsetting("AccessToken") }

func (s *Store) GetExpiredAt() string { return s.getsetting("ExpiredAt") }

func (s *Store) GetAppKey() string { return s.getsetting("AppKey") }

func (s *Store) GetAppID() string { return s.getsetting("AppID") }

func (s *Store) GetSecretKey() string { return s.getsetting("SecretKey") }

func (s *Store) GetChanifyAddr() string { return s.getsetting("ChanifyAddr") }

func (s *Store) GetCloudRoot() string { return s.getsetting("Root") }

func (s *Store) GetCron() string { return s.getsetting("Cron") }

func (s *Store) Tasklist() []Task {
	tasks := make([]Task, 0)
	list, err := s.GetAllTask()
	if err == nil {
		tasks = append(tasks, list...)
	}
	return tasks
}
