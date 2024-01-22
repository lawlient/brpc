import React from 'react';
import { Layout, Menu } from "antd";
import { useNavigate } from 'react-router-dom';
import "./index.css"

const { Sider, Content, Header } = Layout;




const Main = (props) => {
    const navigate = useNavigate();

    return (
        <Layout className='main'>
            <Header className='header' style={{ position: 'sticky', top: 0, zIndex: 1, width: '100%' }}>
                <div>安</div>
                <div style={{ display: "flex" }}>
                    门
                    <div>走</div>
                </div>
            </Header>
            <Layout >
                <Sider width={200} className="site-layout-background" >
                    <Menu
                        mode="inline"
                        selectedKeys={[
                            location.pathname === "/" ? "/admin" : location.pathname,
                        ]}
                        style={{
                            height: '100%',
                            borderRight: 0,
                        }}
                        onClick={(info) => {
                            navigate(info.key)
                        }}
                    >
                        <Menu.Item key="/admin">用户</Menu.Item>
                        <Menu.Item key="/entry">站点</Menu.Item>
                    </Menu>
                </Sider>
                <Layout style={{ padding: "0 24px 24px" }}>
                    <Content
                        className='site-layout-background'
                        style={{
                            padding: 24,
                            margin: 0,
                            minHeight: 280,
                        }}
                    >
                        {props.children}
                    </Content>
                </Layout>
            </Layout>
        </Layout>
    )
}


export default Main;