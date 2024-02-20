package apollo

type Application struct {
	AppID       string `json:"AppID"`
	AppKey      string `json:"AppKey"`
	SecretKey   string `json:"SecretKey"`
	SignKey     string `json:"SignKey"`
	RedirectUri string `json:"RedirectUri`
	AccessToken string `json:"AccessToken"`
}

const ns = "application"

func AppID() string {
	return GetString(ns, "AppID")
}

func AppKey() string {
	return GetString(ns, "AppKey")
}

func SecretKey() string {
	return GetString(ns, "SecretKey")
}

func SignKey() string {
	return GetString(ns, "SignKey")
}

func AccessToken() string {
	return GetString(ns, "AccessToken")
}

func RedirectUri() string {
	return GetString(ns, "RedirectUri")
}

func UploadFiles() string {
	return GetString(ns, "UploadFiles")
}

func UploadSpec() string {
	return GetString(ns, "UploadSpec")
}
