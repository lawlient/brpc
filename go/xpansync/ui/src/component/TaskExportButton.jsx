import { Button } from "primereact/button";


export function TaskExportButton({tasks}) {

    const cols = [
        { field: 'ff', header: 'FF' },
        { field: 'how', header: 'How' },
    ];

    const exportColumns = cols.map((col) => ({ title: col.header, dataKey: col.field }));

    const exportCSV = (selectionOnly) => {
        dt.current.exportCSV({ selectionOnly });
    };

    const exportPdf = () => {
        // import('jspdf').then((jsPDF) => {
        //     import('jspdf-autotable').then(() => {
        //         const doc = new jsPDF.default(0, 0);

        //         doc.autoTable(exportColumns, tasks);
        //         doc.save('tasks.pdf');
        //     });
        // });
    };

    const exportExcel = () => {
      //  import('xlsx').then((xlsx) => {
      //      const worksheet = xlsx.utils.json_to_sheet(tasks);
      //      const workbook = { Sheets: { data: worksheet }, SheetNames: ['data'] };
      //      const excelBuffer = xlsx.write(workbook, {
      //          bookType: 'xlsx',
      //          type: 'array'
      //      });

      //      saveAsExcelFile(excelBuffer, 'tasks');
      //  });
    };

    const saveAsExcelFile = (buffer, fileName) => {
        // import('file-saver').then((module) => {
        //     if (module && module.default) {
        //         let EXCEL_TYPE = 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet;charset=UTF-8';
        //         let EXCEL_EXTENSION = '.xlsx';
        //         const data = new Blob([buffer], {
        //             type: EXCEL_TYPE
        //         });

        //         module.default.saveAs(data, fileName + '_export_' + new Date().getTime() + EXCEL_EXTENSION);
        //     }
        // });
    };

    const items = [
        {
            label: 'CSV',
            icon: 'pi pi-file',
            command: () => exportCSV(false),
        },
        {
            label: 'XLS',
            icon: 'pi pi-excel',
            command: () => exportExcel(),
        },
        {
            label: 'PDF',
            icon: 'pi pi-pdf',
            command: () => exportPdf(),
        },
    ]

    return (
        <Button className="taskcard-toolbar-button" icon='pi pi-file-export' />
    )
}