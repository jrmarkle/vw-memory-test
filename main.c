#include <stdio.h>

#include <vowpalwabbit/vwdll.h>

int main() {
	VW_IOBUF buf;
	char* modelBuf;
	size_t modelBufLen;

	const char* commandLine = "--quiet --loss_function=logistic --link=logistic --confidence --save_resume";

	setbuf(stdout, NULL);
	{
		VW_HANDLE h = VW_InitializeA(commandLine);

		for (int i = 0; i < 1e3; i++) {
			{
				VW_EXAMPLE goodExample = VW_ReadExampleA(h, "1 |Feature Good");
				VW_Learn(h, goodExample);
				VW_FinishExample(h, goodExample);
			}

			{
				VW_EXAMPLE badExample = VW_ReadExampleA(h, "-1 |Feature Bad");
				VW_Learn(h, badExample);
				VW_FinishExample(h, badExample);
			}
		}

		VW_CopyModelData(h, &buf, &modelBuf, &modelBufLen);
		VW_Finish(h);
	}

	for (int i = 0; i < 1e1; i++) {
		printf("%d...\n", i);
		VW_HANDLE h = VW_InitializeWithModel(commandLine, modelBuf, modelBufLen);
		for (int j = 0; j < 1e4; j++) {
			{
				VW_EXAMPLE goodExample = VW_ReadExampleA(h, "|Feature Good");
				float prediction = VW_Learn(h, goodExample);
				float confidence = VW_GetConfidence(goodExample);
				if (prediction < 0.9 || confidence < 10) {
					printf("predict good = %f, confidence = %f\n", prediction, confidence);
					VW_FinishExample(h, goodExample);
					VW_Finish(h);
					return 1;
				}
				VW_FinishExample(h, goodExample);
			}
			{
				VW_EXAMPLE badExample = VW_ReadExampleA(h, "|Feature Bad");
				float prediction = VW_Learn(h, badExample);
				float confidence = VW_GetConfidence(badExample);
				if (prediction > 0.1 || confidence < 10) {
					printf("predict bad = %f, confidence = %f\n", prediction, confidence);
					VW_FinishExample(h, badExample);
					VW_Finish(h);
					return 1;
				}
				VW_FinishExample(h, badExample);
			}
			{
				VW_EXAMPLE unknownExample = VW_ReadExampleA(h, "|Feature Unknown");
				float prediction = VW_Learn(h, unknownExample);
				float confidence = VW_GetConfidence(unknownExample);
				if (prediction > 0.6 || prediction < 0.4 || confidence > 1) {
					printf("predict unknown = %f, confidence = %f\n", prediction, confidence);
					VW_FinishExample(h, unknownExample);
					VW_Finish(h);
					return 1;
				}
				VW_FinishExample(h, unknownExample);
			}
		}
		VW_Finish(h);
	}

	VW_FreeIOBuf(buf);
	return 0;
}
