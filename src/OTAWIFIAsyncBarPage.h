#ifndef OTAWIFIAsyncBarPage_h
#define OTAWIFIAsyncBarPage_h

const char OTAWIFIAsyncBarPage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<title>TestoBox OTA</title>


<style>

body {
    font-family: Arial, sans-serif;
    text-align: center;
    margin-top: 40px;
}

h1 {
    font-size: 24px;
}

input {
    margin: 20px;
}

button {
    padding: 12px 25px;
    font-size: 16px;
}

button:disabled {
    opacity: 0.5;
}

#progress {
    width: 90%;
    max-width: 500px;
    height: 25px;
    border: 1px solid #000;
    margin: 20px auto;
}

#bar {
    width: 0%;
    height: 100%;
    background: #000;
}

#status {
    margin-top: 20px;
    font-size: 18px;
}

</style>

</head>


<body>

<h1>TestoBox OTA</h1>


<form id="uploadForm">

<input
    type="file"
    id="firmware"
    name="firmware"
    accept=".bin"
    required
>

<br>

<button
    type="submit"
    id="uploadButton">

    Update Firmware

</button>

</form>


<div id="progress">

<div id="bar"></div>

</div>


<div id="status">
    Ready
</div>


<script>


// =====================================================
// ELEMENTS
// =====================================================

const form =
    document.getElementById("uploadForm");

const fileInput =
    document.getElementById("firmware");

const uploadButton =
    document.getElementById("uploadButton");

const bar =
    document.getElementById("bar");

const statusText =
    document.getElementById("status");


// =====================================================
// RESET PAGE
// =====================================================

function resetPage() {

    // Wyczyszczenie wybranego pliku
    fileInput.value = "";


    // Wyzerowanie paska
    bar.style.width = "0%";


    // Przywrócenie przycisku
    uploadButton.disabled = false;


    // Stan pocz¹tkowy
    statusText.innerText = "Ready";
}


// =====================================================
// UPLOAD
// =====================================================

form.addEventListener(
    "submit",
    function(event) {

        event.preventDefault();


        const file =
            fileInput.files[0];


        // ---------------------------------------------
        // Brak pliku
        // ---------------------------------------------

        if (!file) {

            statusText.innerText =
                "Select firmware file.";

            return;
        }


        // ---------------------------------------------
        // Przygotowanie
        // ---------------------------------------------

        uploadButton.disabled = true;

        statusText.innerText =
            "Starting update...";

        bar.style.width = "0%";


        const formData =
            new FormData();


        formData.append(
            "firmware",
            file
        );


        const xhr =
            new XMLHttpRequest();


        xhr.open(
            "POST",
            "/update",
            true
        );


        // =================================================
        // UPLOAD PROGRESS
        // =================================================

        xhr.upload.addEventListener(
            "progress",
            function(event) {

                if (event.lengthComputable) {

                    const percent =
                        Math.round(
                            (event.loaded /
                             event.total) * 100
                        );


                    bar.style.width =
                        percent + "%";


                    statusText.innerText =
                        "Uploading: " +
                        percent +
                        "%";
                }
            }
        );


        // =================================================
        // SUCCESS / ERROR RESPONSE
        // =================================================

        xhr.onload = function() {

            // ---------------------------------------------
            // SUCCESS
            // ---------------------------------------------

            if (
                xhr.status === 200 &&
                xhr.responseText === "OK"
            ) {

                bar.style.width = "100%";

                statusText.innerText =
                    "Update successful.";

                return;
            }


            // ---------------------------------------------
            // SERVER ERROR
            // ---------------------------------------------

            statusText.innerText =
                "Update failed.";


            // Krótka chwila z komunikatem
            // i powrót do stanu pocz¹tkowego

            setTimeout(
                function() {

                    resetPage();

                },
                2000
            );
        };


        // =================================================
        // CONNECTION ERROR
        // =================================================

        xhr.onerror = function() {

            statusText.innerText =
                "Connection lost.";


            // Po utracie po³¹czenia nie próbujemy
            // od razu resetowaæ strony.
            //
            // Dajemy 2 sekundy na pokazanie komunikatu.

            setTimeout(
                function() {

                    resetPage();

                },
                2000
            );
        };


        // =================================================
        // ABORT
        // =================================================

        xhr.onabort = function() {

            statusText.innerText =
                "Upload aborted.";


            setTimeout(
                function() {

                    resetPage();

                },
                2000
            );
        };


        // =================================================
        // SEND
        // =================================================

        xhr.send(formData);

    }
);

</script>


</body>

</html>

)rawliteral";

#endif
