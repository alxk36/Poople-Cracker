# Dictionary Scraper

## Source and process

The word list in `dictionary.txt` was made from the main English-word section of the 1906 *Webster's Practical Dictionary*, hosted by the Internet Archive:

- [Scanned dictionary PDF](https://archive.org/download/websterspractica00web/websterspractica00web.pdf)
- [OCR text generated from the scan](https://archive.org/download/websterspractica00web/websterspractica00web_djvu.txt)
- [Internet Archive item metadata](https://archive.org/metadata/websterspractica00web)

The Archive item identifies the book as a 1906 publication and marks it public domain in the United States. The script downloads the OCR text rather than performing OCR on the PDF itself. It scans the main vocabulary section, stopping before the appendix's pronouncing vocabulary of proper names, and keeps lines that look like four-letter, title-case headwords followed by a comma. It lowercases, sorts, and deduplicates the results. Three obvious OCR substitutions (`Xash`, `Xick`, and `Xief`) are corrected to `lash`, `lick`, and `lief`.

The generated list contained 1,552 entries, including `poop`. OCR can still contain errors, so this is not a fully proofread word list.

## Reproduce the file

Run this PowerShell script from the project folder. It writes or replaces `dictionary.txt` in the current folder.

```powershell
$ErrorActionPreference = 'Stop'

$sourceUrl = 'https://archive.org/download/websterspractica00web/websterspractica00web_djvu.txt'
$ocrPath = Join-Path $env:TEMP ("websters-ocr-{0}.txt" -f [guid]::NewGuid().ToString('N'))
$outputPath = Join-Path (Get-Location) 'dictionary.txt'

# These zero-based positions identify the main-entry range in this source OCR file.
$mainStartIndex = 909
$appendixStartIndex = 114255

try {
    Invoke-WebRequest -Uri $sourceUrl -OutFile $ocrPath
    $lines = [System.IO.File]::ReadAllLines($ocrPath)

    if ($lines.Length -le $appendixStartIndex) {
        throw "The downloaded OCR text is shorter than the expected source."
    }
    if ($lines[$mainStartIndex].Trim() -cne 'A') {
        throw "The expected beginning of the main vocabulary was not found."
    }
    if ($lines[$appendixStartIndex] -notmatch 'PRONOUNCING\s+VOCABULARY') {
        throw "The expected appendix boundary was not found."
    }

    $words = [System.Collections.Generic.SortedSet[string]]::new(
        [System.StringComparer]::Ordinal
    )

    for ($i = $mainStartIndex; $i -lt $appendixStartIndex; $i++) {
        if ($lines[$i] -cmatch '^\s*([A-Z][a-z]{3}),\s') {
            $word = $Matches[1].ToLowerInvariant()

            switch ($word) {
                'xash' { $word = 'lash' }
                'xick' { $word = 'lick' }
                'xief' { $word = 'lief' }
            }

            [void]$words.Add($word)
        }
    }

    if (-not $words.Contains('poop')) {
        throw 'The extracted list does not contain the required target word "poop".'
    }

    $wordArray = [string[]]$words
    [System.IO.File]::WriteAllLines(
        $outputPath,
        $wordArray,
        [System.Text.UTF8Encoding]::new($false)
    )

    $writtenWords = [System.IO.File]::ReadAllLines($outputPath)
    $invalidWords = @($writtenWords | Where-Object { $_ -cnotmatch '^[a-z]{4}$' })
    if ($invalidWords.Count -gt 0) {
        throw "The output contains $($invalidWords.Count) invalid line(s)."
    }

    Write-Host "Created $outputPath with $($writtenWords.Length) four-letter words."
}
finally {
    if (Test-Path -LiteralPath $ocrPath) {
        Remove-Item -LiteralPath $ocrPath -Force
    }
}
```
