import { useState } from "react"
import { DataTable } from 'primereact/datatable'
import { Column } from "primereact/column"
import { InputText } from "primereact/inputtext"
import { Tag } from "primereact/tag"
import { Dropdown } from "primereact/dropdown"
import { updtask } from "../api/api"


export function TaskTable({
    tasks, setTasks,
    selected, setSelected,
}) {

    const [hows] = useState(["origin", "package"])

    const onRowEditComplete = (e) => {
        let _list = [...tasks];
        let { newData, index } = e;

        _list[index] = newData;

        updtask(newData).then(res=> {
            setTasks(_list);
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
        <DataTable value={tasks} editMode="row" dataKey="id" 
            selectionMode={'checkbox'}  selection={selected} onSelectionChange={(e) => setSelected(e.value)}
            onRowEditComplete={onRowEditComplete} tableStyle={{ minWidth: '50rem' }} >
            <Column selectionMode="multiple" headerStyle={{ width: '3rem' }}></Column>
            <Column key='ff' field="ff" header="File/Folder" editor={(options) => textEditor(options)}></Column>
            <Column key='how' field="how" header="How" body={howTemplate} editor={(options) => howEditor(options)}></Column>
            <Column rowEditor={true} headerStyle={{ width: '10%', minWidth:'8rem'}} bodyStyle={{ textAlign:'center'}} ></Column>
        </DataTable>
    )
}