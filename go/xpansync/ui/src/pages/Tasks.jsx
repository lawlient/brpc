import { useEffect, useState } from "react"
import { addtask, tasklist, updtask } from "../api/api"
import { Card } from "primereact/card"
import { DataTable } from 'primereact/datatable'
import { Column } from "primereact/column"
import { Button } from "primereact/button"
import { Dialog } from "primereact/dialog"
import { InputText } from "primereact/inputtext"
import { RadioButton } from "primereact/radiobutton"
import "./Tasks.css"
import { Tag } from "primereact/tag"
import { Dropdown } from "primereact/dropdown"

export default function Tasks() {
    const [list, setList] = useState([])
    const [selected, setSelected] = useState(null)
    const [count, setCount] = useState(0)
    const [visib, setVisib] = useState(false)
    const [t, setT] = useState({
        ff: "",
        how: "origin",
    })

    const [hows] = useState(["origin", "package"])

    useEffect(() => {
        tasklist().then(res => {
            setList(res.data.data)
        })
    }, [count])

    const onRowEditComplete = (e) => {
        let _list = [...list];
        let { newData, index } = e;

        _list[index] = newData;

        setList(_list);
        updtask(newData).then(res=> {
            console.log(res)
            setCount(count + 1)
        })
    };

    const textEditor = (options) => {
        return <InputText type="text" value={options.value} onChange={(e) => options.editorCallback(e.target.value)} />;
    };
    const howTemplate = (data) => {
        return <Tag value={data.how} severity={data.how === "origin" ? "primary" : "success"}></Tag>;
    }

    const howEditor = (options) => {
        console.log(options)
        return (
            <Dropdown
                value={options.value}
                options={hows}
                onChange={(e) => options.editorCallback(e.value)}
            />
        );
    };

    return (
        <Card>
        <DataTable value={list} editMode="row" dataKey="id" 
            selectionMode={'checkbox'}  selection={selected} onSelectionChange={(e) => setSelected(e.value)}
            onRowEditComplete={onRowEditComplete} tableStyle={{ minWidth: '50rem' }} >
            <Column selectionMode="multiple" headerStyle={{ width: '3rem' }}></Column>
            <Column key='ff' field="ff" header="File/Folder" editor={(options) => textEditor(options)}></Column>
            <Column key='how' field="how" header="How" body={howTemplate} editor={(options) => howEditor(options)}></Column>
            <Column rowEditor={true} headerStyle={{ width: '10%', minWidth:'8rem'}} bodyStyle={{ textAlign:'center'}} ></Column>
        </DataTable>
        <Button onClick={()=> setVisib(true)}>Add</Button>
        <Button onClick={() => {/* delete confirm */}}>Del</Button>
        <Dialog visible={visib} header="Add Task" style={{ width: '50vw' }} onHide={() => setVisib(false)}>
            <form className="form" onSubmit={(e) => {
                e.preventDefault()
                console.log(t)
                addtask(t).then(res => {
                    setCount(count + 1)
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
        </Card>
    )
}

