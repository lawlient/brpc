import { Card } from "primereact/card";
import { useEffect, useState } from "react";
import { userlist } from "../api/api";
import { DataTable } from "primereact/datatable";
import { Column } from "primereact/column";
import { Tag } from "primereact/tag"

export default function User() {
    const [list, setList] = useState([])
    useEffect(() => {
        userlist().then(res => {
            setList(res.data.data)
        })
    }, [])

    return (
        <Card  >
            <DataTable value={list} key={"id"} >
                <Column field="username" header="Name" ></Column>
                <Column field="role" header="Role" body={(e) => {
                    return <Tag value={e.role} severity={"primary"} ></Tag>
                }}></Column>
            </DataTable>

        </Card>
    )
}