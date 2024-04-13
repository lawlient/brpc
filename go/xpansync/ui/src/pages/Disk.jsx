import "./Disk.css"
import { Card } from "primereact/card";
import { Avatar } from 'primereact/avatar'
import { useEffect, useState } from "react";
import { userinfo, getquota } from "../api/api";
import { Chart } from 'primereact/chart';
import { TokenPanel } from "../component/TokenPanel";



export default function Disk() {
    const [user, setUser] = useState({})
    const [quota, setQuota] = useState({})

    const toGB = (b) => { return Math.ceil(b / (1024 * 1024 * 1024)) }

    useEffect(() => {
        userinfo().then(res => {
            setUser(res.data.data)
        })
    }, [])

    useEffect(() => {
        getquota().then(res => {
            const quota = res.data.data
            const documentStyle = getComputedStyle(document.documentElement);
            var data = {
                labels : ["used", "free", ],
                datasets: [
                    {
                        data: [toGB(quota.used), toGB(quota.total - quota.used)],
                        backgroundColor: [
                            documentStyle.getPropertyValue('--blue-500'), 
                            documentStyle.getPropertyValue('--green-500')
                        ],
                        hoverBackgroundColor: [
                            documentStyle.getPropertyValue('--blue-400'), 
                            documentStyle.getPropertyValue('--green-400')
                        ]
                    }
                ]
            }
            setQuota(data)
        })
    }, [])

    return (
        <div  className="disk">

            <Card className="owner-card">
                <Avatar className="avatar" image={user.avatar_url} shape="Badge" size="xlarge" />
                <p className="name">{user.baidu_name}</p>
            </Card>
            <Card title="Disk Quote" style={{display:'flex', flexDirection:'column', gap: '8px', justifyContent:'center', alignItems:'center'}}>
                <Chart type="doughnut" data={quota} options={{cutout:'60%'}} style={{width:"15rem"}}/>
            </Card>
            <div className="token-card">
                <TokenPanel />
            </div>
        </div>
    )
}