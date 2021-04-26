package com.example.extractor;

public interface ExtractPolicy {
    boolean shouldExtract(android.content.Context context);

    boolean forceOverwrite();

    FileExtractor extractor();
}
