import { useEffect, useState } from "react"
import { Button } from "primereact/button";
import { InputText } from "primereact/inputtext";
import { Dialog } from "primereact/dialog";
import { getcode, newtoken, gettoken } from "../api/api";


export function TokenPanel() {
    const [token, setToken] = useState("")
    const [code, setCode] = useState("")
    const [visib, setVisib] = useState(false)

    useEffect(() => {
        gettoken().then(res => {
            setToken(res.data.data)
        })
    }, [])

    return (
        <div style={{ display:'flex', flexDirection:'row', alignItems:'center', gap: '8px'}}>
            <p style={{backgroundColor:'white', padding:'8px', height:'100%', borderWidth:'2px', borderRadius:'8px', boxShadow:'2px'}}>{token}</p>
            <Button style={{ width: '30px', height: '30px' }} icon='pi pi-sync' onClick={() => {
                getcode().then(res => {
                    console.log(res.data)
                    setVisib(true)
                    window.open(res.data.data)
                })
            }}></Button>

            <Dialog header="Refresh Access Token" visible={visib} onHide={() => { setVisib(false) }} >
                <div className="p-inputgroup flex-2" >
                    <InputText value={code} onChange={(e) => setCode(e.target.value)} placeholder="paste code here" />
                    <Button icon="pi pi-save" onClick={() => {
                        newtoken(code).then(res => {
                            setToken(res.data.data)
                            setCode("")
                            setVisib(false)
                        })
                    }} className="p-button-primary" />
                </div>
            </Dialog>
        </div>
    )
}