package sdk

type Sdk struct {
	Config *config
}

func NewSdk(configfile string) (*Sdk, error) {
	sdk := &Sdk{}

	if err := sdk.InitConfig(configfile); err != nil {
		return nil, err
	}

	return sdk, nil
}
