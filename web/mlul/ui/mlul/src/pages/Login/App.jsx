import React from 'react';
import { useNavigate } from "react-router-dom";
import { LockOutlined, UserOutlined } from '@ant-design/icons';
import { Button, Checkbox, Form, Input, Image, message } from 'antd';
import "./App.css"


const Login = () => {
  const navigate = useNavigate();

  const onLogin = async (values) => {
      const data = await fetch('/login', {
          method: "post",
          mode: "cors",
          headers: {
              'Content-Type':'application/json'
          },
          body: JSON.stringify(values)
      });
      if (data.status == 200) {
          // window.localStorage.setItem("_token", token);
          message.success("登录成功!")
          navigate("/admin")
      } else {
          message.error("登录失败!")
      }
  }
  return (
    <div className="login-form">
      <div className='login-box'>
        <div className='logo' style={{  display: "flex", alignItems: "center",justifyContent:"center",marginBottom: 30,fontSize: 18,fontWeight:500 }}>
            <Image src="https://cdn.icon-icons.com/icons2/2070/PNG/96/music_notes_icon_126250.png" alt="music" preview="false" />
        </div>
        <Form
          name="normal_login"
          initialValues={{
            remember: true,
          }}
          onFinish={onLogin}
          autoComplete="on"
        >
          <Form.Item
            name="name"
            rules={[
              {
                required: true,
                message: 'Please input your Username!',
              },
            ]}
          >
            <Input prefix={<UserOutlined className="site-form-item-icon" />} placeholder="Username" />
          </Form.Item>
          <Form.Item
            name="password"
            rules={[
              {
                required: true,
                message: 'Please input your Password!',
              },
            ]}
          >
            <Input
              prefix={<LockOutlined className="site-form-item-icon" />}
              type="password"
              placeholder="Password"
            />
          </Form.Item>
          <Form.Item>
            <Form.Item name="remember" valuePropName="checked" noStyle>
              <Checkbox>Remember me</Checkbox>
            </Form.Item>

            <a className="login-form-forgot" href="">
              Forgot password
            </a>
          </Form.Item>

          <Form.Item>
            <Button type="primary" htmlType="submit" className="login-form-button" >
              Log in
            </Button>
            Or <a href="">register now!</a>
          </Form.Item>
        </Form>
      </div>
    </div>
  );
};
export default Login;
