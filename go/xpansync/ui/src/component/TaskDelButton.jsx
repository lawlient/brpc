import { Button } from "primereact/button"
import { ConfirmDialog, confirmDialog } from 'primereact/confirmdialog';
import { deltask } from "../api/api";



export function TaskDelButton({ tasks, setTasks, selected }) {
  const accept = () => {
    console.log(selected)
    let _list = tasks
    for (let index = 0; index < selected.length; index++) {
      const t = selected[index];
      deltask(t).then(
        _list = _list.filter((v) => {
          return v.id != t.id
        })
      )
    }
    setTasks(_list)
  }

  return (
    <>
      <ConfirmDialog />
      <Button className="taskcard-toolbar-button" disabled={selected.length === 0} onClick={() => {
        confirmDialog({
          message: 'Do you want to delete this record?',
          header: 'Delete Confirmation',
          icon: 'pi pi-info-circle',
          defaultFocus: 'reject',
          acceptClassName: 'p-button-danger',
          accept,
        })
      }} icon="pi pi-trash" ></Button>
    </>
  )
}