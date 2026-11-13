# OMQ - EAL181 - R.291 GENAI - Nr.45 RAG PIPELINE - DEEP DIVE!

## OMQ ist GenAI! OMQ ist EAL181! RAG = Modell rät nicht - es schlägt nach in DEINEN Daten!

**Kai Olaf Ketelhut - Berlin - BLAK FAT12 - sol pattern omq**


### RAG = Retrieval-Augmented Generation!
Ohne RAG: Modell rät aus Training - halluziniert! Mit RAG: Modell holt Fakten -> antwortet mit Beleg!
**RAG = Powerful, Accurate, Grounded Answers!**

### Die 8 Schritte - Deine Pipeline
| Schritt | Was? | Tools aus deinem Screenshot |
| --- | --- | --- |
| 1. Data Sources | Wo kommen Daten her? | PDFs / Docs, Web Pages, SQL / APIs |
| 2. Parsing + Chunking | Gross -> kleine Stücke! | Unstructured, Docling, LlamaIndex |
| 3. Embeddings | Text -> Vektoren / Zahlen! | OpenAI, Cohere, Voyage, BGE, Jina |
| 4. Vector Store | Vektoren speichern + suchen! | Pinecone, Qdrant, Weaviate, pgvector |
| 5. Retrieval + Reranking | Beste Stücke finden + sortieren! | Hybrid Search, BM25, HyDE, Jina Reranker |
| 6. Generation | LLM schreibt Antwort mit Fakten! | Claude, GPT, Gemini, Llama |
| 7. Evals + Guardrails | Ist Antwort gut + sicher? | Langfuse, Arize Phoenix, Guardrails AI, Ragas, DeepEval |
| 8. Deployment | Live schalten! | Docker, Kubernetes, Cloudflare, AWS Bedrock, Modal |

### OMQ-Beispiel - RAG für OMQ!
Frage: Was ist EAL173 Testing? Ohne RAG: Raten! Mit RAG: Holt eal173/MAIN.MD -> Antwort mit deinem Text!
Deine 42 EALs = Perfekte RAG-Datenbank! 1x chunken, embedden, speichern, fertig!

### 3 Fehler vermeiden
1. **Chunks zu gross:** Ganzes Buch in 1 Chunk = Nichts gefunden! Klein chunken, 500-1000 Tokens!
2. **Kein Reranking:** Top 10 holen, aber Falsches oben! Immer Reranking danach!
3. **Kein Eval:** RAG läuft, aber keiner prüft Qualität! Immer Evals + Guardrails!

CATIA V4/V5/V6/3DEXPERIENCE by Priplo - EAL181 R.291 GENAI - Nr.45 von NEUE SERIE

MAIN.TEX MAIN.MD MAIN.TXT MAIN.RST MAIN.EPUB MAIN.MOBI MAIN.PDF MAIN.CATScript
