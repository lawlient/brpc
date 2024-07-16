import "../pages/Tasks.css"
import { useState } from "react"
import { Button } from "primereact/button"
import { Dialog } from "primereact/dialog"
import { InputText } from "primereact/inputtext"
import { RadioButton } from "primereact/radiobutton"

import { addtask } from "../api/api"

export function TaskAddButton({ tasks, setTasks }) {
  const [visib, setVisib] = useState(false)
  const [t, setT] = useState({
    ff: "",
    how: "origin",
  })

  return (
    <>
      <Button className="taskcard-toolbar-button" onClick={() => setVisib(true)} icon="pi pi-plus"></Button>
      <Dialog visible={visib} header="Add Task" onHide={() => setVisib(false)}>
        <form className="addtask-form" onSubmit={(e) => {
          e.preventDefault()
          addtask(t).then(res => {
            let _list = [t, ...tasks]
            setTasks(_list)
          })
          setVisib(false)
        }}>
          <InputText placeholder="file/folder" onChange={(e) => setT({ ...t, ff: e.target.value })} />
          <div className="addtask-form-howgroup">
            <div>
              <RadioButton onChange={(e) => setT({ ...t, how: "origin" })} checked={t.how === 'origin'} />
              <label>Origin</label>
            </div>
            <div >
              <RadioButton onChange={(e) => setT({ ...t, how: "package" })} checked={t.how === 'package'} />
              <label>Package</label>
            </div>
          </div>
          <Button type="submit" style={{ alignSelf: "center", padding: "8px 15px" }}>Save</Button>
        </form>
      </Dialog>
    </>
  )
}