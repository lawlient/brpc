import "./Tasks.css"
import { useEffect, useState } from "react"
import { tasklist } from "../api/api"
import { Card } from "primereact/card"
import { Toolbar } from "primereact/toolbar"
import { TaskTable } from "../component/TaskTable"
import { TaskAddButton } from "../component/TaskAddButton"
import { TaskDelButton } from "../component/TaskDelButton"
import { TaskExportButton } from "../component/TaskExportButton"

export default function Tasks() {
    const [list, setList] = useState([])
    const [selected, setSelected] = useState([])

    useEffect(() => {
        tasklist().then(res => {
            setList(res.data.data)
        })
    }, [])


    const startTool = () => {
        return (
            <div style={{display:'flex', flexDirection:'row', gap: '8px' }}>
                <TaskAddButton tasks={list} setTasks={setList} />
                <TaskDelButton tasks={list} setTasks={setList} selected={selected} />
                <TaskExportButton tasks={list} />
            </div>
        )
    }

    return (
        <Card>
        <Toolbar start={startTool} />
        <TaskTable tasks={list} setTasks={setList} selected={selected} setSelected={setSelected} />
        </Card>
    )
}

