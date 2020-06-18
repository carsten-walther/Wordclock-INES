/**
 * LanguageServiceController
 */
export class LanguageServiceController {

  /**
   * file
   */
  private file: string = '/assets/data/languages.json';

  /**
   * get
   */
  get = async (): Promise<Language[]> => {
    return fetch(this.file)
      .then(response => response.json())
      .then(data => {
        return data;
      });
  };
}

export const LanguageService = new LanguageServiceController();
