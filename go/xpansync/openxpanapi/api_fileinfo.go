/*
xpan

xpanapi

API version: 0.1
*/

// Code generated by OpenAPI Generator (https://openapi-generator.tech); DO NOT EDIT.

package openapi

import (
	"bytes"
	_context "context"
	_ioutil "io/ioutil"
	_nethttp "net/http"
	_neturl "net/url"
)

// Linger please
var (
	_ _context.Context
)

// FileinfoApiService FileinfoApi service
type FileinfoApiService service

type ApiXpanfiledoclistRequest struct {
	ctx _context.Context
	ApiService *FileinfoApiService
	accessToken *string
	parentPath *string
	recursion *string
	page *int32
	num *int32
	order *string
	desc *string
	web *string
}

func (r ApiXpanfiledoclistRequest) AccessToken(accessToken string) ApiXpanfiledoclistRequest {
	r.accessToken = &accessToken
	return r
}
func (r ApiXpanfiledoclistRequest) ParentPath(parentPath string) ApiXpanfiledoclistRequest {
	r.parentPath = &parentPath
	return r
}
func (r ApiXpanfiledoclistRequest) Recursion(recursion string) ApiXpanfiledoclistRequest {
	r.recursion = &recursion
	return r
}
func (r ApiXpanfiledoclistRequest) Page(page int32) ApiXpanfiledoclistRequest {
	r.page = &page
	return r
}
func (r ApiXpanfiledoclistRequest) Num(num int32) ApiXpanfiledoclistRequest {
	r.num = &num
	return r
}
func (r ApiXpanfiledoclistRequest) Order(order string) ApiXpanfiledoclistRequest {
	r.order = &order
	return r
}
func (r ApiXpanfiledoclistRequest) Desc(desc string) ApiXpanfiledoclistRequest {
	r.desc = &desc
	return r
}
func (r ApiXpanfiledoclistRequest) Web(web string) ApiXpanfiledoclistRequest {
	r.web = &web
	return r
}

func (r ApiXpanfiledoclistRequest) Execute() (string, *_nethttp.Response, error) {
	return r.ApiService.XpanfiledoclistExecute(r)
}

/*
Xpanfiledoclist Method for Xpanfiledoclist

file doclist

 @param ctx _context.Context - for authentication, logging, cancellation, deadlines, tracing, etc. Passed from http.Request or context.Background().
 @return ApiXpanfiledoclistRequest
*/
func (a *FileinfoApiService) Xpanfiledoclist(ctx _context.Context) ApiXpanfiledoclistRequest {
	return ApiXpanfiledoclistRequest{
		ApiService: a,
		ctx: ctx,
	}
}

// Execute executes the request
//  @return string
func (a *FileinfoApiService) XpanfiledoclistExecute(r ApiXpanfiledoclistRequest) (string, *_nethttp.Response, error) {
	var (
		localVarHTTPMethod   = _nethttp.MethodGet
		localVarPostBody     interface{}
		formFiles            []formFile
		localVarReturnValue  string
	)

	localBasePath, err := a.client.cfg.ServerURLWithContext(r.ctx, "FileinfoApiService.Xpanfiledoclist")
	if err != nil {
		return localVarReturnValue, nil, GenericOpenAPIError{error: err.Error()}
	}

	localVarPath := localBasePath + "/rest/2.0/xpan/file?method=doclist&openapi=xpansdk"

	localVarHeaderParams := make(map[string]string)
	localVarQueryParams := _neturl.Values{}
	localVarFormParams := _neturl.Values{}
	if r.accessToken == nil {
		return localVarReturnValue, nil, reportError("accessToken is required and must be specified")
	}

	localVarQueryParams.Add("access_token", parameterToString(*r.accessToken, ""))
	if r.parentPath != nil {
		localVarQueryParams.Add("parent_path", parameterToString(*r.parentPath, ""))
	}
	if r.recursion != nil {
		localVarQueryParams.Add("recursion", parameterToString(*r.recursion, ""))
	}
	if r.page != nil {
		localVarQueryParams.Add("page", parameterToString(*r.page, ""))
	}
	if r.num != nil {
		localVarQueryParams.Add("num", parameterToString(*r.num, ""))
	}
	if r.order != nil {
		localVarQueryParams.Add("order", parameterToString(*r.order, ""))
	}
	if r.desc != nil {
		localVarQueryParams.Add("desc", parameterToString(*r.desc, ""))
	}
	if r.web != nil {
		localVarQueryParams.Add("web", parameterToString(*r.web, ""))
	}
	// to determine the Content-Type header
	localVarHTTPContentTypes := []string{}

	// set Content-Type header
	localVarHTTPContentType := selectHeaderContentType(localVarHTTPContentTypes)
	if localVarHTTPContentType != "" {
		localVarHeaderParams["Content-Type"] = localVarHTTPContentType
	}

	// to determine the Accept header
	localVarHTTPHeaderAccepts := []string{"application/json; charset=UTF-8"}

	// set Accept header
	localVarHTTPHeaderAccept := selectHeaderAccept(localVarHTTPHeaderAccepts)
	if localVarHTTPHeaderAccept != "" {
		localVarHeaderParams["Accept"] = localVarHTTPHeaderAccept
	}
	req, err := a.client.prepareRequest(r.ctx, localVarPath, localVarHTTPMethod, localVarPostBody, localVarHeaderParams, localVarQueryParams, localVarFormParams, formFiles)
	if err != nil {
		return localVarReturnValue, nil, err
	}

	localVarHTTPResponse, err := a.client.callAPI(req)
	if err != nil || localVarHTTPResponse == nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	localVarBody, err := _ioutil.ReadAll(localVarHTTPResponse.Body)
	localVarHTTPResponse.Body.Close()
	localVarHTTPResponse.Body = _ioutil.NopCloser(bytes.NewBuffer(localVarBody))
	if err != nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	if localVarHTTPResponse.StatusCode >= 300 {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: localVarHTTPResponse.Status,
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	err = a.client.decode(&localVarReturnValue, localVarBody, localVarHTTPResponse.Header.Get("Content-Type"))
	if err != nil {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: err.Error(),
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	return localVarReturnValue, localVarHTTPResponse, nil
}

type ApiXpanfileimagelistRequest struct {
	ctx _context.Context
	ApiService *FileinfoApiService
	accessToken *string
	parentPath *string
	recursion *string
	page *int32
	num *int32
	order *string
	desc *string
	web *string
}

func (r ApiXpanfileimagelistRequest) AccessToken(accessToken string) ApiXpanfileimagelistRequest {
	r.accessToken = &accessToken
	return r
}
func (r ApiXpanfileimagelistRequest) ParentPath(parentPath string) ApiXpanfileimagelistRequest {
	r.parentPath = &parentPath
	return r
}
func (r ApiXpanfileimagelistRequest) Recursion(recursion string) ApiXpanfileimagelistRequest {
	r.recursion = &recursion
	return r
}
func (r ApiXpanfileimagelistRequest) Page(page int32) ApiXpanfileimagelistRequest {
	r.page = &page
	return r
}
func (r ApiXpanfileimagelistRequest) Num(num int32) ApiXpanfileimagelistRequest {
	r.num = &num
	return r
}
func (r ApiXpanfileimagelistRequest) Order(order string) ApiXpanfileimagelistRequest {
	r.order = &order
	return r
}
func (r ApiXpanfileimagelistRequest) Desc(desc string) ApiXpanfileimagelistRequest {
	r.desc = &desc
	return r
}
func (r ApiXpanfileimagelistRequest) Web(web string) ApiXpanfileimagelistRequest {
	r.web = &web
	return r
}

func (r ApiXpanfileimagelistRequest) Execute() (string, *_nethttp.Response, error) {
	return r.ApiService.XpanfileimagelistExecute(r)
}

/*
Xpanfileimagelist Method for Xpanfileimagelist

file imagelist

 @param ctx _context.Context - for authentication, logging, cancellation, deadlines, tracing, etc. Passed from http.Request or context.Background().
 @return ApiXpanfileimagelistRequest
*/
func (a *FileinfoApiService) Xpanfileimagelist(ctx _context.Context) ApiXpanfileimagelistRequest {
	return ApiXpanfileimagelistRequest{
		ApiService: a,
		ctx: ctx,
	}
}

// Execute executes the request
//  @return string
func (a *FileinfoApiService) XpanfileimagelistExecute(r ApiXpanfileimagelistRequest) (string, *_nethttp.Response, error) {
	var (
		localVarHTTPMethod   = _nethttp.MethodGet
		localVarPostBody     interface{}
		formFiles            []formFile
		localVarReturnValue  string
	)

	localBasePath, err := a.client.cfg.ServerURLWithContext(r.ctx, "FileinfoApiService.Xpanfileimagelist")
	if err != nil {
		return localVarReturnValue, nil, GenericOpenAPIError{error: err.Error()}
	}

	localVarPath := localBasePath + "/rest/2.0/xpan/file?method=imagelist&openapi=xpansdk"

	localVarHeaderParams := make(map[string]string)
	localVarQueryParams := _neturl.Values{}
	localVarFormParams := _neturl.Values{}
	if r.accessToken == nil {
		return localVarReturnValue, nil, reportError("accessToken is required and must be specified")
	}

	localVarQueryParams.Add("access_token", parameterToString(*r.accessToken, ""))
	if r.parentPath != nil {
		localVarQueryParams.Add("parent_path", parameterToString(*r.parentPath, ""))
	}
	if r.recursion != nil {
		localVarQueryParams.Add("recursion", parameterToString(*r.recursion, ""))
	}
	if r.page != nil {
		localVarQueryParams.Add("page", parameterToString(*r.page, ""))
	}
	if r.num != nil {
		localVarQueryParams.Add("num", parameterToString(*r.num, ""))
	}
	if r.order != nil {
		localVarQueryParams.Add("order", parameterToString(*r.order, ""))
	}
	if r.desc != nil {
		localVarQueryParams.Add("desc", parameterToString(*r.desc, ""))
	}
	if r.web != nil {
		localVarQueryParams.Add("web", parameterToString(*r.web, ""))
	}
	// to determine the Content-Type header
	localVarHTTPContentTypes := []string{}

	// set Content-Type header
	localVarHTTPContentType := selectHeaderContentType(localVarHTTPContentTypes)
	if localVarHTTPContentType != "" {
		localVarHeaderParams["Content-Type"] = localVarHTTPContentType
	}

	// to determine the Accept header
	localVarHTTPHeaderAccepts := []string{"application/json; charset=UTF-8"}

	// set Accept header
	localVarHTTPHeaderAccept := selectHeaderAccept(localVarHTTPHeaderAccepts)
	if localVarHTTPHeaderAccept != "" {
		localVarHeaderParams["Accept"] = localVarHTTPHeaderAccept
	}
	req, err := a.client.prepareRequest(r.ctx, localVarPath, localVarHTTPMethod, localVarPostBody, localVarHeaderParams, localVarQueryParams, localVarFormParams, formFiles)
	if err != nil {
		return localVarReturnValue, nil, err
	}

	localVarHTTPResponse, err := a.client.callAPI(req)
	if err != nil || localVarHTTPResponse == nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	localVarBody, err := _ioutil.ReadAll(localVarHTTPResponse.Body)
	localVarHTTPResponse.Body.Close()
	localVarHTTPResponse.Body = _ioutil.NopCloser(bytes.NewBuffer(localVarBody))
	if err != nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	if localVarHTTPResponse.StatusCode >= 300 {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: localVarHTTPResponse.Status,
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	err = a.client.decode(&localVarReturnValue, localVarBody, localVarHTTPResponse.Header.Get("Content-Type"))
	if err != nil {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: err.Error(),
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	return localVarReturnValue, localVarHTTPResponse, nil
}

type ApiXpanfilelistRequest struct {
	ctx _context.Context
	ApiService *FileinfoApiService
	accessToken *string
	dir *string
	folder *string
	start *string
	limit *int32
	order *string
	desc *string
	web *string
	showempty *int32
}

func (r ApiXpanfilelistRequest) AccessToken(accessToken string) ApiXpanfilelistRequest {
	r.accessToken = &accessToken
	return r
}
func (r ApiXpanfilelistRequest) Dir(dir string) ApiXpanfilelistRequest {
	r.dir = &dir
	return r
}
func (r ApiXpanfilelistRequest) Folder(folder string) ApiXpanfilelistRequest {
	r.folder = &folder
	return r
}
func (r ApiXpanfilelistRequest) Start(start string) ApiXpanfilelistRequest {
	r.start = &start
	return r
}
func (r ApiXpanfilelistRequest) Limit(limit int32) ApiXpanfilelistRequest {
	r.limit = &limit
	return r
}
func (r ApiXpanfilelistRequest) Order(order string) ApiXpanfilelistRequest {
	r.order = &order
	return r
}
func (r ApiXpanfilelistRequest) Desc(desc string) ApiXpanfilelistRequest {
	r.desc = &desc
	return r
}
func (r ApiXpanfilelistRequest) Web(web string) ApiXpanfilelistRequest {
	r.web = &web
	return r
}
func (r ApiXpanfilelistRequest) Showempty(showempty int32) ApiXpanfilelistRequest {
	r.showempty = &showempty
	return r
}

func (r ApiXpanfilelistRequest) Execute() (string, *_nethttp.Response, error) {
	return r.ApiService.XpanfilelistExecute(r)
}

/*
Xpanfilelist Method for Xpanfilelist

file list

 @param ctx _context.Context - for authentication, logging, cancellation, deadlines, tracing, etc. Passed from http.Request or context.Background().
 @return ApiXpanfilelistRequest
*/
func (a *FileinfoApiService) Xpanfilelist(ctx _context.Context) ApiXpanfilelistRequest {
	return ApiXpanfilelistRequest{
		ApiService: a,
		ctx: ctx,
	}
}

// Execute executes the request
//  @return string
func (a *FileinfoApiService) XpanfilelistExecute(r ApiXpanfilelistRequest) (string, *_nethttp.Response, error) {
	var (
		localVarHTTPMethod   = _nethttp.MethodGet
		localVarPostBody     interface{}
		formFiles            []formFile
		localVarReturnValue  string
	)

	localBasePath, err := a.client.cfg.ServerURLWithContext(r.ctx, "FileinfoApiService.Xpanfilelist")
	if err != nil {
		return localVarReturnValue, nil, GenericOpenAPIError{error: err.Error()}
	}

	localVarPath := localBasePath + "/rest/2.0/xpan/file?method=list&openapi=xpansdk"

	localVarHeaderParams := make(map[string]string)
	localVarQueryParams := _neturl.Values{}
	localVarFormParams := _neturl.Values{}
	if r.accessToken == nil {
		return localVarReturnValue, nil, reportError("accessToken is required and must be specified")
	}

	localVarQueryParams.Add("access_token", parameterToString(*r.accessToken, ""))
	if r.dir != nil {
		localVarQueryParams.Add("dir", parameterToString(*r.dir, ""))
	}
	if r.folder != nil {
		localVarQueryParams.Add("folder", parameterToString(*r.folder, ""))
	}
	if r.start != nil {
		localVarQueryParams.Add("start", parameterToString(*r.start, ""))
	}
	if r.limit != nil {
		localVarQueryParams.Add("limit", parameterToString(*r.limit, ""))
	}
	if r.order != nil {
		localVarQueryParams.Add("order", parameterToString(*r.order, ""))
	}
	if r.desc != nil {
		localVarQueryParams.Add("desc", parameterToString(*r.desc, ""))
	}
	if r.web != nil {
		localVarQueryParams.Add("web", parameterToString(*r.web, ""))
	}
	if r.showempty != nil {
		localVarQueryParams.Add("showempty", parameterToString(*r.showempty, ""))
	}
	// to determine the Content-Type header
	localVarHTTPContentTypes := []string{}

	// set Content-Type header
	localVarHTTPContentType := selectHeaderContentType(localVarHTTPContentTypes)
	if localVarHTTPContentType != "" {
		localVarHeaderParams["Content-Type"] = localVarHTTPContentType
	}

	// to determine the Accept header
	localVarHTTPHeaderAccepts := []string{"application/json; charset=UTF-8"}

	// set Accept header
	localVarHTTPHeaderAccept := selectHeaderAccept(localVarHTTPHeaderAccepts)
	if localVarHTTPHeaderAccept != "" {
		localVarHeaderParams["Accept"] = localVarHTTPHeaderAccept
	}
	req, err := a.client.prepareRequest(r.ctx, localVarPath, localVarHTTPMethod, localVarPostBody, localVarHeaderParams, localVarQueryParams, localVarFormParams, formFiles)
	if err != nil {
		return localVarReturnValue, nil, err
	}

	localVarHTTPResponse, err := a.client.callAPI(req)
	if err != nil || localVarHTTPResponse == nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	localVarBody, err := _ioutil.ReadAll(localVarHTTPResponse.Body)
	localVarHTTPResponse.Body.Close()
	localVarHTTPResponse.Body = _ioutil.NopCloser(bytes.NewBuffer(localVarBody))
	if err != nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	if localVarHTTPResponse.StatusCode >= 300 {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: localVarHTTPResponse.Status,
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	err = a.client.decode(&localVarReturnValue, localVarBody, localVarHTTPResponse.Header.Get("Content-Type"))
	if err != nil {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: err.Error(),
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	return localVarReturnValue, localVarHTTPResponse, nil
}

type ApiXpanfilesearchRequest struct {
	ctx _context.Context
	ApiService *FileinfoApiService
	accessToken *string
	key *string
	web *string
	num *int32
	page *int32
	dir *string
	recursion *string
}

func (r ApiXpanfilesearchRequest) AccessToken(accessToken string) ApiXpanfilesearchRequest {
	r.accessToken = &accessToken
	return r
}
func (r ApiXpanfilesearchRequest) Key(key string) ApiXpanfilesearchRequest {
	r.key = &key
	return r
}
func (r ApiXpanfilesearchRequest) Web(web string) ApiXpanfilesearchRequest {
	r.web = &web
	return r
}
func (r ApiXpanfilesearchRequest) Num(num int32) ApiXpanfilesearchRequest {
	r.num = &num
	return r
}
func (r ApiXpanfilesearchRequest) Page(page int32) ApiXpanfilesearchRequest {
	r.page = &page
	return r
}
func (r ApiXpanfilesearchRequest) Dir(dir string) ApiXpanfilesearchRequest {
	r.dir = &dir
	return r
}
func (r ApiXpanfilesearchRequest) Recursion(recursion string) ApiXpanfilesearchRequest {
	r.recursion = &recursion
	return r
}

func (r ApiXpanfilesearchRequest) Execute() (string, *_nethttp.Response, error) {
	return r.ApiService.XpanfilesearchExecute(r)
}

/*
Xpanfilesearch Method for Xpanfilesearch

file search

 @param ctx _context.Context - for authentication, logging, cancellation, deadlines, tracing, etc. Passed from http.Request or context.Background().
 @return ApiXpanfilesearchRequest
*/
func (a *FileinfoApiService) Xpanfilesearch(ctx _context.Context) ApiXpanfilesearchRequest {
	return ApiXpanfilesearchRequest{
		ApiService: a,
		ctx: ctx,
	}
}

// Execute executes the request
//  @return string
func (a *FileinfoApiService) XpanfilesearchExecute(r ApiXpanfilesearchRequest) (string, *_nethttp.Response, error) {
	var (
		localVarHTTPMethod   = _nethttp.MethodGet
		localVarPostBody     interface{}
		formFiles            []formFile
		localVarReturnValue  string
	)

	localBasePath, err := a.client.cfg.ServerURLWithContext(r.ctx, "FileinfoApiService.Xpanfilesearch")
	if err != nil {
		return localVarReturnValue, nil, GenericOpenAPIError{error: err.Error()}
	}

	localVarPath := localBasePath + "/rest/2.0/xpan/file?method=search&openapi=xpansdk"

	localVarHeaderParams := make(map[string]string)
	localVarQueryParams := _neturl.Values{}
	localVarFormParams := _neturl.Values{}
	if r.accessToken == nil {
		return localVarReturnValue, nil, reportError("accessToken is required and must be specified")
	}
	if r.key == nil {
		return localVarReturnValue, nil, reportError("key is required and must be specified")
	}

	localVarQueryParams.Add("access_token", parameterToString(*r.accessToken, ""))
	localVarQueryParams.Add("key", parameterToString(*r.key, ""))
	if r.web != nil {
		localVarQueryParams.Add("web", parameterToString(*r.web, ""))
	}
	if r.num != nil {
		localVarQueryParams.Add("num", parameterToString(*r.num, ""))
	}
	if r.page != nil {
		localVarQueryParams.Add("page", parameterToString(*r.page, ""))
	}
	if r.dir != nil {
		localVarQueryParams.Add("dir", parameterToString(*r.dir, ""))
	}
	if r.recursion != nil {
		localVarQueryParams.Add("recursion", parameterToString(*r.recursion, ""))
	}
	// to determine the Content-Type header
	localVarHTTPContentTypes := []string{}

	// set Content-Type header
	localVarHTTPContentType := selectHeaderContentType(localVarHTTPContentTypes)
	if localVarHTTPContentType != "" {
		localVarHeaderParams["Content-Type"] = localVarHTTPContentType
	}

	// to determine the Accept header
	localVarHTTPHeaderAccepts := []string{"application/json; charset=UTF-8"}

	// set Accept header
	localVarHTTPHeaderAccept := selectHeaderAccept(localVarHTTPHeaderAccepts)
	if localVarHTTPHeaderAccept != "" {
		localVarHeaderParams["Accept"] = localVarHTTPHeaderAccept
	}
	req, err := a.client.prepareRequest(r.ctx, localVarPath, localVarHTTPMethod, localVarPostBody, localVarHeaderParams, localVarQueryParams, localVarFormParams, formFiles)
	if err != nil {
		return localVarReturnValue, nil, err
	}

	localVarHTTPResponse, err := a.client.callAPI(req)
	if err != nil || localVarHTTPResponse == nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	localVarBody, err := _ioutil.ReadAll(localVarHTTPResponse.Body)
	localVarHTTPResponse.Body.Close()
	localVarHTTPResponse.Body = _ioutil.NopCloser(bytes.NewBuffer(localVarBody))
	if err != nil {
		return localVarReturnValue, localVarHTTPResponse, err
	}

	if localVarHTTPResponse.StatusCode >= 300 {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: localVarHTTPResponse.Status,
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	err = a.client.decode(&localVarReturnValue, localVarBody, localVarHTTPResponse.Header.Get("Content-Type"))
	if err != nil {
		newErr := GenericOpenAPIError{
			body:  localVarBody,
			error: err.Error(),
		}
		return localVarReturnValue, localVarHTTPResponse, newErr
	}

	return localVarReturnValue, localVarHTTPResponse, nil
}
