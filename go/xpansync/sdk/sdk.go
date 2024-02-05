package sdk

type sdk struct {
	config *config
}

func NewSdk(configfile string) (*sdk, error) {
	sdk := &sdk{}

	if err := sdk.InitConfig(configfile); err != nil {
		return nil, err
	}

	return sdk, nil
}
