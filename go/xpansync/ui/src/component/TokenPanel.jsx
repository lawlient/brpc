import "./TokenPanel.css"
import { useEffect, useState } from "react"
import { Button } from "primereact/button";
import { InputText } from "primereact/inputtext";
import { Dialog } from "primereact/dialog";
import { getcode, newtoken, gettoken } from "../api/api";

function day(n) { return Math.floor(n / 86400); }
function hour(n) { return Math.floor((n / 3600 + 16) % 24); }
function minute(n) { return Math.floor(n % 3600 / 60); }
function second(n) { return Math.floor(n % 3600 % 60 % 60); }

export function TokenPanel() {
  const [token, setToken] = useState("")
  const [life, setLife] = useState(0)
  const [code, setCode] = useState("")
  const [visib, setVisib] = useState(false)

  useEffect(() => {
    gettoken().then(res => {
      setToken(res.data.data)
      setLife(res.data.life)
    })
  }, [])

  useEffect(() => {
    const timer = setInterval(() => {
      setLife(life - 1)
    }, 1000)

    return () => clearInterval(timer)
  }, [life])

  return (
    <div className="token-panel">
      <p className="hint">
        <i className="pi pi-key" />
        <span>  {day(life)}d {hour(life)}h {minute(life)}m {second(life)}s</span>
      </p>
      <button onClick={() => {
        getcode().then(res => {
          console.log(res.data)
          setVisib(true)
          window.open(res.data.data)
        })
      }}>
        <i className="pi pi-sync"></i>
      </button>

      <Dialog header="Refresh Access Token" visible={visib} onHide={() => { setVisib(false) }} >
        <div className="p-inputgroup flex-2" >
          <InputText value={code} onChange={(e) => setCode(e.target.value)} placeholder="paste code here" />
          <Button icon="pi pi-save" onClick={() => {
            newtoken(code).then(res => {
              setToken(res.data.data)
              setLife(res.data.life)
              setCode("")
              setVisib(false)
            })
          }} className="p-button-primary" />
        </div>
      </Dialog>
    </div>
  )
}