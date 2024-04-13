import { useState } from "react"
import { Button } from "primereact/button"
import { Dialog } from "primereact/dialog"
import { InputText } from "primereact/inputtext"
import { RadioButton } from "primereact/radiobutton"

import { addtask } from "../api/api"


export function TaskAddButton({tasks, setTasks}) {
    const [visib, setVisib] = useState(false)
    const [t, setT] = useState({
        ff: "",
        how: "origin",
    })

    return (
        <>
        <Button onClick={()=> setVisib(true)} icon="pi pi-plus" ></Button>
        <Dialog visible={visib} header="Add Task" style={{ width: '50vw' }} onHide={() => setVisib(false)}>
            <form className="form" onSubmit={(e) => {
                e.preventDefault()
                addtask(t).then(res => {
                    let _list = [t, ...tasks]
                    setTasks(_list)
                })
                setVisib(false)
            }}>
                <InputText placeholder="file/folder" onChange={(e) => setT({...t, ff:e.target.value})} />
                <div className="howgroup">
                    <div className="flex align-items-center">
                        <RadioButton onChange={(e) => setT({...t, how:"origin"})} checked={t.how === 'origin'} />
                        <label className="ml-2">Origin</label>
                    </div> 
                    <div className="flex align-items-center">
                        <RadioButton onChange={(e) => setT({...t, how:"package"})} checked={t.how === 'package'} />
                        <label className="ml-2">Package</label>
                    </div> 
                </div>
                <Button type="submit" style={{ alignSelf:"center" }}>Save</Button>
            </form>
        </Dialog>
        </>
    )
}