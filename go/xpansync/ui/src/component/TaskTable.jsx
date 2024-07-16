import "./TaskTable.css"
import { useState } from "react"
import { Tag } from "primereact/tag"
import { updtask } from "../api/api"
import { InputText } from "primereact/inputtext"
import { Button } from "primereact/button"


export function TaskTable({
  tasks, setTasks,
  selected, setSelected,
}) {

  const [newrow, setNewrow] = useState()

  const hows = [
    { label: "origin", color:  "primary"},
    { label: "package", color:  "success"},
  ]

  const onRowEditComplete = (data, i) => {
    let _list = [...tasks];
    _list[i] = data;

    updtask(data).then(res => {
      setTasks(_list);
      setNewrow(undefined)
    })
  };

  const textEditor = (data) => {
    return <InputText className="textinput" type="text" defaultValue={data.ff} 
      onChange={(e) => {
        let cpy = data
        cpy.ff = e.target.value
        setNewrow(cpy)
      }} />;
  };

  const findhow = (l) => {
    for (let i = 0; i < hows.length; i++) {
      if (hows[i].label === l) {
        return hows[i]
      }
    }
  }
  const howTemplate = (how) => {
    let h = findhow(how)
    return <Tag className="howtag" value={how} severity={h.color}></Tag>;
  }

  const howEditor = (data) => {
    return (
      <select defaultValue={data.how} onChange={(e) => {
        let cpy = data
        cpy.how = e.target.value
        setNewrow(cpy)
      }}>
      {hows.map((h, i) => {
        return <option key={i} value={h.label}>{h.label}</option>
      })}
      </select>
    );
  };

  const add = (e) => {
    for (let i = 0; i < selected.length; i++) {
      if (selected[i] === e) {
        return
      }
    }
    let cpy = [...selected, e]
    setSelected(cpy)
  }
  const del = (e) => {
    let cpy = selected.filter((v) => {
      return v.id != e.id
    })
    setSelected(cpy)
  }
  const addall = () => { setSelected(tasks) }
  const delall = () => { setSelected([]) }
  const choosed = (e) => {
    for (let i = 0; i < selected.length; i++){
      if (selected[i].id === e.id) return true
    }
    return false
  }

  return (
    <div className="tasktable">
      <div className="tasktable-head tasktable-row">
        <div>
          <input type="checkbox" style={{width:'18px',height:'18px'}} onChange={(e) => {
            e.target.checked ? addall() : delall()
          }}/>
        </div>
        <div>File/Folder</div>
        <div>How</div>
        <div>Op</div>
      </div>
      {
        tasks.map((li, idx) => {
          const edit = (newrow?.id === li.id);
          return (
            <div key={idx} className="tasktable-row">
              <div>
                <input type="checkbox" checked={choosed(li)} style={{width:'18px',height:'18px'}} onChange={(e) => {
                  e.target.checked ? add(li) : del(li)
                }}/>
              </div>
              <div>{edit ? textEditor(newrow) : li.ff}</div>
              <div>{edit ? howEditor(newrow) : howTemplate(li.how)}</div>
              {edit
              ? <div>
                  <Button icon="pi pi-check" onClick={() => onRowEditComplete(newrow, idx)}></Button>
                  <Button icon="pi pi-times" onClick={() => setNewrow(undefined)}></Button>
                </div>
              : <div>
                  <Button icon="pi pi-pencil" onClick={() => setNewrow(li)}></Button>
                </div>
              }
            </div>
          )
        })
      }
    </div>
  )
}