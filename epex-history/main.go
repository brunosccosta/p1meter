package main

import (
    "encoding/json"
    "net/http"
	"git.snt.utwente.nl/dingen/cloudburst/scrapers/easyenergy"
    "time"
    "log"
)

func main() {
    // preload data
    tarrifs := loadData()

    http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
        // Set the response content type to JSON
        log.Println("handling request")

        w.Header().Set("Content-Type", "application/json")

        // Encode the foos slice as JSON and write it to the response writer
        json.NewEncoder(w).Encode(tarrifs)
    })

    // Start the HTTP server on port 8080
    http.ListenAndServe(":8090", nil)
}

func loadData() []easyenergy.EnergyTariff {
    log.Println("[PreLoad] starting")

    http := http.Client{}

    epex := easyenergy.NewClient(&http)

    start := time.Date(2021, 1, 1, 0, 0, 0, 0, time.UTC)
    end := time.Date(2023, 1, 1, 0, 0, 0, 0, time.UTC)

    data, err := epex.Get(start, end)
    if err != nil {
        log.Fatal("Can't read data", err)
        return nil
    }

    log.Println("[PreLoad] loaded", len(data))
    log.Println("[PreLoad] done")

    return data
}